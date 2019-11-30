/* An Implicit Free List heap allocator created by Akram Sbaih on May 31, 2019.
 * It relies on headers pointing to the next blocks and containing the status of
 * the block in the first three bits (for error checking) in a circular form.
 */ 
#include "allocator.h"
#include "debug_break.h"
#include <string.h>

#define headerSize (size_t)sizeof(size_t)
#define minBlockSize headerSize + ALIGNMENT
#define minHeapSize minBlockSize
#define maskSetFree 0x7UL         // 0000...00111 (or it)
#define maskSetUsed ~maskSetFree  // 1111...11000 (and it)
#define maskCheckFree maskSetFree // 0000...00111 (and it, non-0 if free)
#define maskGetSize maskSetUsed   // 1111...11000 (and it, it removes the status bits)

static size_t *heapBegin = NULL, *heapEnd = NULL; // will store pointers of the boundaries of the heap
static size_t availableSize = 0; // number of bytes free (without headers)

/* Function: roundup
 * -----------------
 * This function rounds up the given number to the given multiple, which
 * must be a power of 2, and returns the result.
 */
size_t roundup(size_t sz) {
    return (sz + ALIGNMENT-1) & ~(ALIGNMENT-1);
}

/* This function is to be called at the beginning of each session
 * It initializes the structure of the heap
 */
bool myinit(void *heap_start, size_t heap_size) {
    if(heap_size < minHeapSize) return false;
    heapBegin = heap_start;
    heapEnd = (void *)((char *)heapBegin + heap_size);
    // Make a header for the first free block
    availableSize = heap_size - headerSize;
    *heapBegin = availableSize | maskSetFree;
    return true;
}

/* This function simulate the standard 'malloc' function in that it returns a 
 * pointer to an allocated block of at least the requested size
 */
void *mymalloc(size_t rsize) {
    rsize = roundup(rsize); // for alignment
    // reject requests with impossible sizes or when myinit is not called
    if(!rsize || rsize > availableSize || rsize > MAX_REQUEST_SIZE) return NULL;
    size_t curr_size = 0;
    size_t* curr = heapBegin;
    while (curr < heapEnd) {
        curr_size = *curr & maskGetSize;
        if(!(*curr & maskCheckFree) ||  curr_size < rsize) {
            // this updates the value if curr_size only if needed to spare computation
            // block is not free or smaller than needed, skip
            curr = (size_t*)((char*)curr +  headerSize + curr_size);
            continue;
        }
        // Starting here, this block has enough space for this request

        // this line is to avoid fragmentation where there are free blocks of size 0
        if(curr_size - rsize < minBlockSize) rsize = curr_size;
        if(curr_size > rsize) { // create a new free block
            size_t* newHeader = (size_t*)((char*)curr + headerSize + rsize);
            *newHeader = curr_size - rsize - headerSize; // the size of the new free block
            *newHeader |= maskSetFree;
            availableSize -= headerSize;
        }
        *curr = rsize & maskSetUsed; // update current block
        availableSize -= rsize;
        return (char *)curr + headerSize; // the beginning of this block's payload
    }
    return NULL; // no good block found
}

/* This function simulates the standard 'free' fiunction in that it frees a block
 * of the address provided by ptr.
 */
void myfree(void *ptr) {
    char* p = ptr; // one time casting instead of casting every line.
    // pointer out of bound or not aligned (NULL is included)
    if((size_t*)p < heapBegin || (size_t*)p > heapEnd || (size_t)p % ALIGNMENT != 0) return;
    //    breakpoint();
    size_t* header = (size_t*)((char *)ptr - headerSize);
    *header |= maskSetFree;
    availableSize += *header & maskGetSize;
}

/* This function simulates the standard 'myrealloc' function in that it changes the 
 * size of a block previosuly provided by this allocator. It has a few edge
 * cases described in the first two lines.
 */
void *myrealloc(void *old_ptr, size_t new_size) {
    if(!old_ptr) return mymalloc(new_size); // when called on a NULL pointer, it's a malloc
    if(!new_size) { myfree(old_ptr); return NULL; } // when called on size 0, it's a free
    void* newBlock = mymalloc(new_size);
    if(!newBlock) return NULL; // no room available
    size_t* oldHeader = (size_t*)((char *)old_ptr - headerSize);
    // the number of bytes to copy is the minimum of the two blocks
    size_t numBytesToMove = *oldHeader & maskGetSize;
    if(new_size < numBytesToMove) numBytesToMove = new_size;
    memmove(newBlock, old_ptr, numBytesToMove); // move old data
    myfree(old_ptr);
    return newBlock;
}

/* Thus function checks the validity of the internal structures of the
 * heap and returns false if it's corrupted.
 */
bool validate_heap() {
    size_t* curr = heapBegin;
    size_t totalFree = 0UL;
    while(curr < heapEnd) {
        // check the status flags are not broken
        if((*curr & maskCheckFree) != maskCheckFree && (*curr & maskCheckFree) != 0) return false;
        if((*curr & maskCheckFree)) totalFree += (*curr & maskGetSize);
        curr = (size_t*)((char *)curr + headerSize + (*curr & maskGetSize));
    }
    if(availableSize != totalFree) return false;
    if(curr != heapEnd) {
        return false;
    }
    return true;
}
