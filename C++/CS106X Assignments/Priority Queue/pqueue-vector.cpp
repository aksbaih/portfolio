#include "pqueue-vector.h"

using namespace std;

/* No constructer or destructer is needed for this class */
VectorPQueue::VectorPQueue() { logSize = 0; }
VectorPQueue::~VectorPQueue() {}

/* This helper function finds the index of the minimum entry in vec and returns its index
 * The size of vec must be larger than zero before calling the function, such errors are not expected or handled
 */
int VectorPQueue::findMinIndex() const {
    int minIndex = 0;
    for(int index = 1; index < vec.size(); index ++) // iterate over all values in the vec
        if(vec[index] < vec[minIndex]) minIndex = index; // to find the smallest value
    return minIndex;
}

/* This function returns the next value in the queue (the smallest) but does not remove it
 * It throws an error if the size of the queue is zero
 */
const string& VectorPQueue::peek() const {
    if(vec.size() == 0) error("Peeking on an empty queue");
    return vec[findMinIndex()];
}

/* This function returns the next value in the queue (the smallest) and removes it shrinking the size of the queue by 1
 * It throws an error if the size of the queue is zero when called
 */
string VectorPQueue::extractMin() {
    if(vec.size() == 0) error("Extracting from an empty queue");
    int index = findMinIndex();
    string value = vec[index]; // stores the value to be returned after it's removed from vec
    vec.remove(index);
    logSize --; // one element is removed -> size shrinks.
    return value;
}

/* This function receives a string and adds it to the queue by adding it to the end of the vector
 * It increases the size of the queue by one
 */
void VectorPQueue::enqueue(const string& value) {
    vec.add(value);
    logSize ++; // one element is added
}

/* This function adds the contents of a queue (q1) to another one (q2) and destroys them
 * it returns a pointer to the new queue
 */
VectorPQueue *VectorPQueue::merge(VectorPQueue * q1, VectorPQueue * q2) {
    VectorPQueue *result = new VectorPQueue();
    result->vec += q1->vec; // add the contents of both queues to the new one
    result->vec += q2->vec;
    result->logSize = result->vec.size();
    delete q1; delete q2; // destroys the queues pointed to by q1 and q2
    return result;
}
