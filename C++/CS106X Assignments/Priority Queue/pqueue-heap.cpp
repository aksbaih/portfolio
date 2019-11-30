#include "pqueue-heap.h"
#include <cmath>
using namespace std;

HeapPQueue::HeapPQueue() {
    array = new string[5]; // skipping the first entry of index 0
    capacity = 4;
    logSize = 0;
}

HeapPQueue::~HeapPQueue() {
    delete [] array;
}

/* This function is needed when there's no room for new elements so a new array is created with a larger size (by theory)
 * newSize is expected to be larger than logSize at least. Error is not expected nor handled.
 */
void HeapPQueue::expandArray(int newSize) {
    string *newArray = new string[newSize+1];
    for(int i = 0; i <= logSize; i++) { // to move the contents of array to newArray
        newArray[i] = array[i];
    }
    delete [] array;
    array = newArray;
    capacity = newSize;
}

/* Swaps the contents of two strings through two pointers
 */
void HeapPQueue::swap(string& a, string& b) {
    string tmp = a;
    a = b;
    b = tmp;
}

/* enforces the heap properities (heapifies) a root down recursively until it reaches a leaf
 */
void HeapPQueue::heapify(int current) {
    int leftChild = current * 2;
    if(leftChild > logSize) return; // this is a leaf, it's heapified by definition
    int rightChild = leftChild + 1;
    int toBeSwapped = leftChild;
    if(rightChild <= logSize && array[rightChild] < array[leftChild]) toBeSwapped = rightChild;

    if(array[toBeSwapped] < array[current]) { // for smallest child
        swap(array[toBeSwapped], array[current]);
        heapify(toBeSwapped);
    }
}

/* This function starts with a given child and heapify its parent and its parent's parents recursively in a line until it reaches
 * the top root. This is useful when inserting a new item at the end of the tree
 */
void HeapPQueue::heapifyParentsRecursive(int current) {
    if(current == 1)  // base case 1 doesn't have parents
        return;
    int parent = current / 2;
    if(array[current] < array[parent]) {
        swap(array[current], array[parent]);
        heapifyParentsRecursive(parent);
    }
}

/* heapifies all lower levels starting from a specific level recursively. This is useful when merging an item by starting from top root
 */
void HeapPQueue::heapifyLevelsRecursive(int level) {
    int startPos = pow(2, level);
    int endPos = pow(2, level+1) -1;
    if(endPos < logSize) // levels are the horizantal rows in the binary map, the last element can be calculated with the formula provided
        heapifyLevelsRecursive(level + 1);
    else return;
    for(int currentlyHeapfying = startPos; currentlyHeapfying <= endPos; currentlyHeapfying ++)
        heapify(currentlyHeapfying);
}

/* returns the smallest value in the queue (the first item in the array
 */
const string& HeapPQueue::peek() const {
    if(logSize == 0) error ("The queue is empty, can't peek!");
    return array[1]; // Top root element is the smallest
}

/* returns the smallest value and removes it from the tree by swapping it with the last item and heapifying the whole tree
 */
string HeapPQueue::extractMin() {
    if(logSize == 0) error ("The queue is empty, can't extract!");
    if(logSize == 1) return array[logSize--]; // when the root is the only element, return it and set logSize to 0
    string result = array[1]; // the top root is the minimum
    array[1] = array[logSize--]; // swap first and last element and remove the last from the list by shtinking logSize
    heapify(1);
    return result;
}

/* enqueues a new element by appending it at the end of the array and heapifying it all level by level
 */
void HeapPQueue::enqueue(const string& value) {
    if(logSize == capacity) expandArray(capacity + 100); // array ran out of room, it is expanded by adding 100 more empty spots
    array[++logSize] = value; //appending the value as the last element
    heapifyParentsRecursive(logSize);
}

/* merges two queues by concatinating their enternal arrays and heapifying their levels recursively. It deletes the original onesm,
 */
HeapPQueue *HeapPQueue::merge(HeapPQueue * q1, HeapPQueue * q2) {
    HeapPQueue *product = new HeapPQueue();
    product->expandArray(q1->logSize + q2->logSize);
    product->logSize = 0;
    for(int i = 1; i <= q1->logSize; i++) // copy the contents of both arrays concatenating them into one
        product->array[++(product->logSize)] = q1->array[i];
    for(int a = 1; a <= q2->logSize; a++)
        product->array[++(product->logSize)] = q2->array[a];
    product->heapifyLevelsRecursive(0);
    delete q1; delete q2;
    return product;
}
