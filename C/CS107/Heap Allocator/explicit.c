/* This is Akram Sbaih's implementation of a heap allocator for CS107 assignment 7
 * It relies on a binary heap tree structure to provide O(log2 N) best-fit allocation
 * and supports in-place reallocation and coalescing. More in the readme.txt file.
 */
#include "allocator.h"
#include "debug_break.h"
#include <string.h>

/* the struct representing a block in the heap. It can be either free or
 * used, indicated by the bits of sizeLoad.
 */
typedef struct header {
    size_t sizeLoad; // the size of the payload and flags on the 3 least significant bits
    struct header* pointers[2];
    // the children of this node if it's free or the address of the left free node if it's used
} header;

#define headerSize sizeof(header)
#define minBlockSize headerSize + ALIGNMENT 

#define maskSetFree 0x1UL        // 00..001 (or it)
#define maskSetUsed ~maskSetFree // 11..110 (and it)
#define maskCheckFree maskSetFree         // 00..001 (and it and get non-zero if free)
#define maskIsolateFlags 0x7     // 00..111 (and it to isolate, or it to concatentate)
#define maskRemoveFlags ~maskIsolateFlags
#define maskSetFirstChild 0x4    // 00..100 (or it to make this a first child)
#define maskSetSecondChild ~maskSetFirstChild // 11..011 (and it to make this a second child)
#define maskCheckFirstChild maskSetFirstChild // 00..100 (and it, non-zero if it's a first child)

header* root = NULL; // the root node of the binary heap tree of free nodes
void *heapBegin, *heapEnd; // where the heap starts and finishes
size_t numFreeBlocks = 0; // number of elements in the binary heap tree

/* This function returns the ceiling multiple of ALIGNMENT to a given number (sz). Copied from lab.
 */
size_t roundup(size_t sz) {
    return (sz + ALIGNMENT-1) & ~(ALIGNMENT-1);
}

/* Helper Functions:
 * The following functions are to update or check the components of a given header as described by their names
 */
void updateSize(header* h, size_t size) {
    h->sizeLoad = size | (h->sizeLoad & maskIsolateFlags);
}
void updateFree(header* h, bool isfree) {
    if(isfree) {
        h->sizeLoad |= maskSetFree;
    } else {
        h->sizeLoad &= maskSetUsed;
    }
}
void updateLeftFree(header* h, header* left) {
    h->pointers[0] = left;
}
size_t checkSize(header* h) {
    return h->sizeLoad & maskRemoveFlags;
}
bool checkFree(header* h) {
    return h->sizeLoad & maskCheckFree;
}
header* checkLeftFree(header* h) {
    return h->pointers[0];
}
void setParent(header* h, header* parent) {
    // This function should be only used with free nodes where it sets the first
    // block of the payload as a pointer to its parent in the binary heap
    if(!h) return; // this node doesn't exist
    *(header**)((char *)h + sizeof(header)) = parent;
}
void setFirstChild(header* h, bool isFirstChild) {
    if(isFirstChild) {
        h->sizeLoad |= maskSetFirstChild;
    } else {
        h->sizeLoad &= maskSetSecondChild;
    }
}
void setChildOrder(header* h, int o) {
    setFirstChild(h, !o);
}
int checkChildOrder(header* h) {
    // returns the pointer index of the node in its parent. 0 (false) :first
    if(h->sizeLoad & maskSetFirstChild) return 0;
    else return 1;
}
header** getParentPtr(header* h) {
    return (header**)((char *)h + sizeof(header));
}
header* getRightBorder(header* h) {
    return (header*)((char *)h + sizeof(header) + checkSize(h));
}

/* This function initializes the first node of the tree (root) with the entire heap size after
 * performing checks on size. The first node has no children and no parent.
 */
bool myinit(void *heap_start, size_t heap_size) {
    if(!heap_start || heap_size < minBlockSize || heap_size % ALIGNMENT) return false;
    root = heap_start;
    root->sizeLoad = heap_size - headerSize; // available memory
    updateFree(root, true);
    root->pointers[0] = NULL; // no children
    root->pointers[1] = NULL;
    setParent(root, NULL); // has no parent
    setFirstChild(root, true);
    heapBegin = heap_start;
    heapEnd = (char *)heap_start + heap_size;
    numFreeBlocks = 1;
    return true;
}

/* This function recursively searches the binary heap to find the best fit for 
 * a requested size by traversing down the tree (top is smallest and grows downwards)
 */
header* findBestFit(size_t rsize, header* node) {
    if(!node) return NULL; // this node does not exist
    if(checkSize(node) >= rsize) return node; // this node is best fit
    header* a = findBestFit(rsize, node->pointers[0]); // check left
    header* b = findBestFit(rsize, node->pointers[1]); // check right
    if(!a) return b; // if a is NULL (b might be NULL, too)
    if(!b) return a; // if b is NULL return a
    return (checkSize(a) < checkSize(b) ? a : b); // the smallest of the children
}

/* This function rewires the tree swapping the virtual positions of two
 * provided nodes. They have to be a parent and a child in the order provided.
 * This function is more effecient than the following one under these restrictions
 */
void swapNodes(header* parent, header* child) {
    // sibling is the other child of parent and has the index that child doesn't have
    header* sibling = parent->pointers[checkChildOrder(child) ? 0 : 1];
    // copy child's children to parent
    memmove((char *)parent + sizeof(parent->sizeLoad), (char *)child + sizeof(child->sizeLoad), 2 * sizeof(header*));
    // copy parent as the new parent of child's children
    setParent(parent->pointers[0], parent);
    setParent(parent->pointers[1], parent);
    // copy parent's parent to child
    setParent(child, *getParentPtr(parent));
    // copy parent's birth order to child
    setChildOrder(child, checkChildOrder(parent));
    if(*getParentPtr(child)) (*getParentPtr(child))->pointers[checkChildOrder(child)] = child;
    // copy child as parent's parent
    setParent(parent, child);
    // copy parent and sibling as child's children with their order
    if(sibling) {
        setFirstChild(parent, checkChildOrder(sibling)); // the complement of the sibling
        setParent(sibling, child);
        child->pointers[checkChildOrder(sibling)] = sibling;
    } else {
        setFirstChild(parent, true);
        child->pointers[1] = NULL; // no sibling
    }
    child->pointers[checkChildOrder(parent)] = parent;
    if(root == parent) root = child;
}

/* Another implementation of the swap function above that works generally for any two nodes
 * not only parents and children. It involves more steps and memory.
 */
void swapNonRelatives(header* one, header* two) {
    header* tmpP[2]; int tmpI;
    header *pone=*getParentPtr(one), *ptwo=*getParentPtr(two); // pointers to parents
    if(two == pone) {swapNodes(two, one); return;} // two is one's parent
    if(one == ptwo) {swapNodes(one, two); return;} // one is two's parent
    // swap parent relations
    if(pone)pone->pointers[checkChildOrder(one)] = two;
    if(ptwo)ptwo->pointers[checkChildOrder(two)] = one;
    tmpI = checkChildOrder(one);
    setChildOrder(one, checkChildOrder(two));
    setChildOrder(two, tmpI);
    setParent(one, ptwo);
    setParent(two, pone);
    // swap children relations
    memmove(tmpP, (char *)one + sizeof(one->sizeLoad), 2 * sizeof(header*));
    memmove((char *)one + sizeof(one->sizeLoad), (char *)two + sizeof(two->sizeLoad), 2 * sizeof(header*));
    memmove((char *)two + sizeof(two->sizeLoad), tmpP, 2 * sizeof(header*));
    if(one->pointers[0]) setParent(one->pointers[0], one);
    if(one->pointers[1]) setParent(one->pointers[1], one);
    if(two->pointers[0]) setParent(two->pointers[0], two);
    if(two->pointers[1]) setParent(two->pointers[1], two);
    // update the root position
    if(root == one) root = two;
    else if(root == two) root = one;
}

/* This function unwires a node out of the binary heap tree by removing it from its
 * parent. It does not deal with the physical heap or heap size. It's important to 
 * remove h's children before call!
 */
void deleteNode(header* h) {
    // set h's parent pointer to it to be NULL
    header* parent = *getParentPtr(h);
    if(parent) parent->pointers[checkChildOrder(h)] = NULL;
    else root = NULL; // it's the old root if it doesn't have a parent
}

/* This function returns the parent of a child by its index in the binary
 * heap tree by recursively tranversing the tree using numFreeNodes
 * It assumes that the structure of the tree is a valid binary heap.
 * Index starts at 1!
 */
header* findChildParent(size_t index) {
    header* h = root;
    int log2index = 0;
    for(; index>>log2index; log2index++); // finds the smallest power of 2 larger than index
    if(log2index < 3) return h; // edge case to protect from underflow of log2index
    size_t mask = 0x1 << (log2index - 1);
    // this mask will point at bits in the index starting from the one to the right of the most significant
    // these bits encode the path in the binary heap tree to the item of the provided index.
    while( (mask >>= 1) != 1 ) { // ignore the least significant bit because we don't want the child
        h = h->pointers[ (mask & index) ? 1 : 0 ];
    }
    return h;
}

/* This is a helper function for 'sink' that returns a pointers to the smaller child.
 * If one of the children is NULL, it returns the other, or NULL if both.
 */
header* smallerChild(header* h, size_t size) {
    if(!h->pointers[0]) return NULL;
    if(!h->pointers[1]) return checkSize(h->pointers[0]) < size ? h->pointers[0] : NULL;
    if(checkSize(h->pointers[0]) >= size && checkSize(h->pointers[1]) >= size) return NULL;
    return (checkSize(h->pointers[0]) < checkSize(h->pointers[1])) ? h->pointers[0] : h->pointers[1];
}

/* This function takes a pointer to a node that increased in size to sink it
 * down the heap tree. It does that by recursively comparing it to its children
 * It's assumed that the inputs provided are not NULL
 */
void heapfySink(header* h, size_t size) {
    // we are using the left child always because it's always guarenteed
    // that there's a left child even if there isn't a right child
    header* child = smallerChild(h, size);
    while(child) {
        // This loop stops when child is NULL (reached the bottom of the tree)
        // or when the child is bigger than the node sinking (it reached its position)
        swapNodes(h, child);
        child = smallerChild(h, size);
    }
}

/* This function takes a pointer to a node that shrank in size to float it
 * up the heap tree. It does that by recursively comparing it to its parents
 */
void heapfyFloat(header* h, size_t size) {
    header* parent = *getParentPtr(h);
    while(parent && checkSize(parent) > size) {
        // this loop stops when parent is NULL or when it's smaller than the node (complys with heap)
        // The parent is bigger than node, swap them in the tree structure:
        swapNodes(parent, h);
        parent = *getParentPtr(h);
    }
    if(!parent) {
        // h floated to the top of the tree (root) because it has no parents
        root = h;
    }
}

/* This function calls either 'heapfySink' or 'heapfyFloat' to position
 * the node provided in the correct place after changing its size
 */
void heapfy(header* h, size_t size) {
    header* parent = *getParentPtr(h);
    if(parent && checkSize(parent) > size) {
        // the parent is bigger than the node, it shrank in size
        heapfyFloat(h, size);
    } else {
        // it didn't shrink, it might have expanded
        heapfySink(h, size);
    }
    if(!(*getParentPtr(h))) root = h; // doesn't have a parent, it's the new root
}

/* This function removes a free block from the binary heap tree by swaping it
 * with the last child of the tree and heapfying this swapped child.
 */
void removeFree(header* h) {
    header* originalLastChild = findChildParent(numFreeBlocks); // parent of last child, will be changed next line
    header* checkLastChild = h;
    originalLastChild = originalLastChild->pointers[originalLastChild->pointers[1] ? 1 : 0]; // rightmost non-NULL child
    if(originalLastChild != h)  {
        swapNonRelatives(originalLastChild, h); // there's no need to swap if they are the same entity
        checkLastChild = findChildParent(numFreeBlocks); // parent of last child, will be changed next line
        checkLastChild = checkLastChild->pointers[checkLastChild->pointers[1] ? 1 : 0]; // rightmost non-NULL child
        if(checkLastChild != h) swapNonRelatives(checkLastChild, h); // there's no need to swap if they are the same entity
    }
    deleteNode(h);
    if(h != checkLastChild){
        heapfy(checkLastChild, checkSize(checkLastChild));
    }
    if(h != originalLastChild) heapfy(originalLastChild, checkSize(originalLastChild));
    numFreeBlocks--;
}

/* takes in (necessarily) two free blocks and merges them, updating the physical and virtual heap
 * It merges the right into the left so the left vanishes
 */
void coalesceFree(header* left, header* right) {
    size_t totalSize = checkSize(left) + checkSize(right) + sizeof(header);
    header* rightBorder = getRightBorder(right); // a pointer to the bordering right USED block
    if((void*)rightBorder < heapEnd && !checkFree(rightBorder)) {
        updateLeftFree(rightBorder, left);
    }
    removeFree(right); // unwire the right free block
    updateSize(left, totalSize);
    heapfy(left, totalSize);
}


/* This function physically repositions the remainder of a free block
 * and virtually by calling 'heapfy'.
 */
void cutFree(header* h, size_t new_size) {
    heapfy(h, new_size);
    header* newHeader = (header*)((char *)h + (checkSize(h) - new_size));
    updateSize(newHeader, new_size);
    updateFree(newHeader, true);
    setChildOrder(newHeader, checkChildOrder(h));
    // copy two children and a parent (stored in the first 8 bytes of free payload
    // from the old block to the new one
    memmove((char *)newHeader + sizeof(newHeader->sizeLoad), (char *)h + sizeof(h->sizeLoad), 3 * sizeof(header*));
    if(newHeader->pointers[0]) // it has a first child, set its payload to point to the new location
        *getParentPtr(newHeader->pointers[0]) = newHeader;
    if(newHeader->pointers[1]) // it has a second child, set its payload to point to the new location
        *getParentPtr(newHeader->pointers[1]) = newHeader;
    header* parent = *getParentPtr(newHeader);
    if(parent) // if this block has a parent, update its child (this element) pointer to 'this'
        parent->pointers[checkChildOrder(newHeader)] = newHeader;
    // update the right bordering USED block to point here
    header* rightBorder = getRightBorder(newHeader);
    if((void*)rightBorder < heapEnd) {
        if(checkFree(rightBorder)) {
            // coalesce; it updates right border automatically
            coalesceFree(newHeader, rightBorder);
        }
        else updateLeftFree(rightBorder, newHeader);
    }
    // In case this was the root
    if(root == h) root = newHeader;
}

/* This function adds a new node to the heap tree by wiring it as the last child
 * and heapfying it up if necessary. It assumes root is not NULL.
 */
void addFree(header* h, size_t size) {
    numFreeBlocks++;
    header* parent = findChildParent(numFreeBlocks);
    bool isFirstChild = !(parent->pointers[0]); // check if there's a sibling or this is the first child

    setFirstChild(h, isFirstChild);
    setParent(h, parent);
    parent->pointers[isFirstChild ? 0 : 1] = h;
    heapfy(h, size);
    // update right bordering USED block to point here
    header* rightBorder = getRightBorder(h);
    if((void*)rightBorder < heapEnd && !checkFree(rightBorder)) updateLeftFree(rightBorder, h);
}


/* This is a helper function for 'mymalloc' and 'myrealloc' that takes a free block of memory and
 * a requested size, does surgery on the heap, and returns a pointer to the beginning of the USED
 * block it allocated. If there's extra room, it frees it.
 */
void *mallocHelper(header* newHeader, size_t rsize) {
    size_t availableBytes = checkSize(newHeader);
    // avoid fragmentation leading to memory loss
    if(availableBytes - rsize < minBlockSize) rsize = availableBytes;
    if(availableBytes > rsize) {
        // there's more room than needed, create a new free block
        cutFree(newHeader, availableBytes - rsize - headerSize);
    } else { // there's no extra room, the free block will vanish
        removeFree(newHeader);
        header* rightBlock = getRightBorder(newHeader);
        if((void*)rightBlock < heapEnd && !checkFree(rightBlock)) updateLeftFree(rightBlock, NULL);
    }
    updateSize(newHeader, rsize);
    updateFree(newHeader, false);
    // since all free blocks are coalesced at all times, and that the free block cuts to the right
    // it means that the block to the left is no way FREE
    updateLeftFree(newHeader, NULL);
    return (char *)newHeader + sizeof(header);
}

/* this function simulates standard 'malloc'. It allocates memory of the provided size and returns its
 * address. If it fails, it returns NULL.
 */
void *mymalloc(size_t rsize) {
    rsize = roundup(rsize); // for alignment
    // reject requests with impossible sizes or when myinit is not called or the heap is completely full
    if(!rsize || rsize > MAX_REQUEST_SIZE || !root) return NULL;
    header* best = findBestFit(rsize, root);
    if(!best) return NULL; // no free block with enough space is found
    // a memory block is found, allocate it:
    return mallocHelper(best, rsize);
}

/* This is a helper function for myfree and myrealloc. It frees the memory at the provided location
 * coalescing it with any right-side free memory, but not left-side
 */
header *myfreeNoLeftCoalesce(header *h) {
    updateFree(h, true);
    h->pointers[0] = h->pointers[1] = NULL; // no children yet
    addFree(h, checkSize(h));
    // coalesce right
    header* rightBorder = getRightBorder(h);
    if((void*)rightBorder < heapEnd && checkFree(rightBorder)) coalesceFree(h, rightBorder);
    else rightBorder = h;
    return rightBorder;
}

/* This function simulates the standard 'free' function. If it's provided with a valid pointer to a
 * block returned by the allocator, it frees it. Otherwise, the behavior is undefined if ptr!=NULL.
 */
void myfree(void *ptr) {
    if(!ptr || ptr < heapBegin || ptr >= heapEnd) return;
    header* h = (header*)((char *)ptr - sizeof(header));
    header* leftBorder = checkLeftFree(h); // this information will vanish later
    // coalesce right
    header* rightBorder = myfreeNoLeftCoalesce(h);
    // coalesce left
    if(leftBorder && checkFree(leftBorder)) coalesceFree(leftBorder, h);
    else leftBorder = h;
    // update the pointer of this block in it's right USED bordering block
    rightBorder = getRightBorder(leftBorder);
    if((void*)rightBorder < heapEnd) updateLeftFree(rightBorder, leftBorder);
}

/* This function simulates the standard 'realloc' function in that it takes a pointer to block
 * previously provided by this allocator and changes its size. A few edge cases are described in the
 * first few lines of the function. Behavior undefined is the pointer doesn't meet the criteria
 */
void *myrealloc(void *old_ptr, size_t new_size) {
    new_size = roundup(new_size); // for alignment
    // reject requests with impossible sizes or when myinit is not called or the heap is completely full
    if(new_size > MAX_REQUEST_SIZE) return NULL;
    if(!new_size) { myfree(old_ptr); return NULL; } // realloc of 0 is a free command
    if(!old_ptr) { return mymalloc(new_size); } // realloc of NULL is a malloc
    
    header* oldHeader = (header*)((char *)old_ptr - sizeof(header)); // the pointer won't change
    size_t oldSize = checkSize(oldHeader); // the oldSize won't change
    size_t potentialInPlace = oldSize; // available bytes after potential inplace coalescing
    header* rightBorder = getRightBorder(oldHeader);
    size_t tmpHeaderComp = *(size_t*)old_ptr; // the first 8 bytes of payload change in the following operations
    if((void*)rightBorder < heapEnd && checkFree(rightBorder)) potentialInPlace += sizeof(header) + checkSize(rightBorder);
    if(potentialInPlace >= new_size) { // can realloc in-place
        header* pointer0Copy = oldHeader->pointers[0]; // a backup for the left pointer (left FREE)
        myfreeNoLeftCoalesce(oldHeader); // free and coalesce this block
        mallocHelper(oldHeader, new_size);
        *(size_t*)old_ptr = tmpHeaderComp;
        oldHeader->pointers[0] = pointer0Copy;
        return old_ptr;
    }
    
    // It's impossible to realloc in-place, move it elsewhere:
    header* best = findBestFit(new_size, root);
    header* leftFreeBorder = checkLeftFree(oldHeader);
    if(leftFreeBorder) potentialInPlace += checkSize(leftFreeBorder) + sizeof(header);
    if(!best && potentialInPlace < new_size) return NULL; // don't free for robustness
    myfree(old_ptr);
    best = findBestFit(new_size, root);
    if(!best) return NULL; // no free block with enough space is found
    // a memory block is found, allocate it:
    size_t numBytesToCopy = oldSize < new_size ? oldSize : new_size;
    memmove((char *)best + sizeof(header) + sizeof(tmpHeaderComp), (char *)oldHeader + sizeof(header) + sizeof(tmpHeaderComp), numBytesToCopy - sizeof(tmpHeaderComp));
    best = mallocHelper(best, new_size);
    *(size_t*)(best) = tmpHeaderComp; // restore first 8 bytes
    return best;
}

/* This is a recursive function that checks multiple properities of the binary heap tree
 * mentioned below are met or not.
 */
bool validateBinaryHeap(header* h, size_t* blocks, size_t* size, int* ones) {
    if(!h) return true;
    (*blocks)++;
    (*size)+= checkSize(h);
    header* parent = *getParentPtr(h);
    if(parent && checkSize(parent) > checkSize(h)) return false; // order corrupted
    if(h->pointers[1] && !h->pointers[0]) return false; // right child without a left one
    if(h->pointers[0] && h->pointers[1] && h->pointers[1]->pointers[0] && !h->pointers[0]->pointers[0]) return false; // unbalanced tree
    if(h->pointers[0] && !h->pointers[1]) (*ones)++; // one-childer
    if(!validateBinaryHeap(h->pointers[0], blocks, size, ones)) return false;
    if(!validateBinaryHeap(h->pointers[1], blocks, size, ones)) return false;
    return true;
}

/* This function is used to assess the structure of the heap allocator
 * It returns false if the structure mismatches what it's supposed to be
 * I.e. the heap is corrupted.
 */
bool validate_heap() {
    if(!root && !numFreeBlocks) return true;
    header* curr = heapBegin;
    size_t totalFreeBlocks = 0;
    size_t totalFreeSpace = 0;
    bool lastFree = false;
    while((void*)curr < heapEnd) {
        if(checkFree(curr)) {
            totalFreeBlocks++;
            totalFreeSpace += checkSize(curr);
            if(lastFree) return false; // two consecutive non-coalesced free blocks
            if((void*)getRightBorder(curr) < heapEnd && checkLeftFree(getRightBorder(curr)) != curr) return false; // not pointed to by the right-side USED
            lastFree = true;
        } else { // block USED
            if(checkLeftFree(curr)) {
                if(!checkFree(curr->pointers[0])) {
                    return false; // flag says left is free, but it's not
                }
            }
            lastFree = false;
        }
        curr = (header*)((char *)curr + sizeof(header) + checkSize(curr));
    }
    size_t binaryHeapTotalBytes = 0;
    size_t binaryHeapTotalBlocks = 0;
    int numOneChilders = 0;
    if(!validateBinaryHeap(root, &binaryHeapTotalBlocks, &binaryHeapTotalBytes, &numOneChilders)) return false;
    if(numOneChilders > 1) return false;
    if(binaryHeapTotalBytes != totalFreeSpace || binaryHeapTotalBlocks != totalFreeBlocks) return false;
    return true;
}
