#include "pqueue-linked-list.h"
using namespace std;

/* constructor function initializes head and tail to nullptr
 */
LinkedListPQueue::LinkedListPQueue() {
    head = tail = nullptr;
    logSize = 0;
}
/* destructor function frees up the memory of the linked list
 */
LinkedListPQueue::~LinkedListPQueue() {
    node * toBeDeleted = head;
    while(toBeDeleted != tail) { // this only happens when the list is not empty and for every node except the last one (tail) because its 'next' is nullptr
        toBeDeleted = toBeDeleted->sisters[1]; // selects the next node
        delete toBeDeleted->sisters[0]; // deletes the current node (previous of next)
    }
    if(tail != nullptr) delete tail; // deletes the tail if the initial list is not empty
}

/* peek returns the value of the first element (smallest) in the linked list pointed to by 'head'
 * this function does not modify the linked list
 */
const string& LinkedListPQueue::peek() const {
    if(logSize < 1) error("Queue is empty.");
    return head->value; // the smallest item is stored in the head node
}

/* extractMin returns the smallest value (head) and deltes its node after chamging the 'head' and 'tail' as necessary for the removal
 */
string LinkedListPQueue::extractMin() {
    if(logSize < 1) error("Queue is empty.");
    string value = head->value;
    node * toBeDeleted = head;          // head is the highest priority
    head = toBeDeleted->sisters[1];     // moves the head pointer to the next item
    if(head == nullptr) tail = nullptr; // happens for the last item which is both pointed to by head and tail (when removed, both should be null)
    delete toBeDeleted;
    logSize --;
    return value;
}

/* enqueue introduces a new element to the linked list. The code of this function is a bit different from a simple search algorithm
 * in that it starts searching from the last element entered to the left or to the right. This way, it doesn't start from the beginning
 * which means that it has to move over less elements in general while searching in a normal distribution.
 */
void LinkedListPQueue::enqueue(const string& value) {
    node * newNode = new node();
    newNode->value = value;
    if(logSize != 0) { // not the first item
        if(value < head->value) { // to be the left most item
            head->sisters[0] = newNode;
            newNode->sisters[1] = head;
            newNode->sisters[0] = nullptr;
            head = newNode;
        } else if(value > tail->value) { // to be the right most item
            tail->sisters[1] = newNode;
            newNode->sisters[0] = tail;
            newNode->sisters[1] = nullptr;
            tail = newNode;
        } else { // the item is somewhere in between
            int dir = (value < lastElemStored->value) ? 0 : 1; // 0 to move left, 1 to move right
            node * toBeNext; // will store the node that the new item will be stored before
            for(toBeNext = lastElemStored->sisters[dir]; (toBeNext != nullptr) && ((dir == 0) ? (value < toBeNext->value) : (value > toBeNext->value)); toBeNext = toBeNext->sisters[dir]);
            /* ^^^  this for loop starts with the sister of the last item inserted in the direction of comparison
             *      it keeps moving in the direction of comparison 0: left (smaller) 1: right (bigger)
             *      it stops when a nullptr is reached (leftmost or rightmost) or when:
             *          - while moving right: the inserted item is bigger or equal to 'toBeNext'
             *          - while moving left : the inserted item is smaller or equal to 'toBeNext'
             *              In the last case, toBeNext will point to the item toBeBefore, so this will be fixed later down...
             *  This search is expected to do less iterations than a common linear search over a symmeteric distribution such as a normal
             *  distribution or a uniform distribution and on generally increasing or generally decreasing distributions:
             *  |           .   |.                  |       .           |
             *  |      .        |    .              |    .     .        |
             *  |   .           |        .          |  .         .      |.............
             *  | .             |          .        | .           .     |
             *  |.              |           .       |.             .    |
             *  |-------------  |--------------     |-----------------  |--------------
             */
            if(dir == 0) toBeNext = toBeNext->sisters[1]; // this is the case mentioned in the last comment
            newNode->sisters[0] = toBeNext->sisters[0];
            newNode->sisters[1] = toBeNext;
            toBeNext->sisters[0]->sisters[1] = newNode;
            toBeNext->sisters[0] = newNode;
        }
    } else { // the first item to be inserted into the list
        head = tail = newNode;
        newNode->sisters[0] = newNode->sisters[1] = nullptr;
    }
    logSize ++;
    lastElemStored = newNode;
}

/* merge returns a pointer to a new queue that has all the values of both a and b and deletes a and b
 * this function is implemented in a way similar to a binary search merge
 */
LinkedListPQueue *LinkedListPQueue::merge(LinkedListPQueue * a, LinkedListPQueue * b) {
    LinkedListPQueue * result = new LinkedListPQueue();
    node * tmp = new node(); // this temporary node is added as a front node to avoid the special case when the 'result' is still empty
    tmp->sisters[0] = tmp->sisters[1] = nullptr;
    result->tail = tmp;
    node * aPtr = a->head; // iterator over elements in a
    node * bPtr = b->head; // iterator over elements in b
    while(aPtr != nullptr && bPtr != nullptr) { // as long as there are still elements in both a and b
        node * * toAdd = (aPtr->value < bPtr->value) ? &aPtr : &bPtr; // ptr to a ptr pointing at the smaller front value in either 'a' or 'b'
        result->tail->sisters[1] = *toAdd;
        (*toAdd)->sisters[0] = result->tail;
        result->tail = *toAdd;
        *toAdd = (*toAdd)->sisters[1]; // advance either aPtr or bPtr by a step (depending on the value of 'toAdd')
    }
    while(aPtr != nullptr) { // to use any remaining elements in a after b is exhausted
        result->tail->sisters[1] = aPtr;
        aPtr->sisters[0] = result->tail;
        result->tail = aPtr;
        aPtr = aPtr->sisters[1];
    }
    while(bPtr != nullptr) { // to use any remaining elements in b after a is exhausted
        result->tail->sisters[1] = bPtr;
        bPtr->sisters[0] = result->tail;
        result->tail = bPtr;
        bPtr = bPtr->sisters[1];
    }
    result->head = tmp->sisters[1]; // prepare to delete the temporary node by skipping 'head' over it
    delete tmp;
    result->logSize = a->logSize + b->logSize;
    a->head = a->tail = b->head = b->tail = nullptr; // so that 'delete' destructors of both 'a' and 'b' don't delete all the nodes
    a->logSize = b->logSize = 0;
    delete a; delete b;
    return result;
}

