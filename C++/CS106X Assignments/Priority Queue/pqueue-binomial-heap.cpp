#include "pqueue-binomial-heap.h"
using namespace std;

BinomialHeapPQueue::BinomialHeapPQueue() {
    logSize = 0;
}
BinomialHeapPQueue::~BinomialHeapPQueue() {}

/* returns the smallest value in the heap by calling finMinDegree and axessing the value it indexes
 */
const string& BinomialHeapPQueue::peek() const {
    if(logSize < 1) error("Queue is empty.");
    return heapTrees[findMinDegree()]->elem;
}

/* returns the minimum value deleting its tree from the heap and adding its child trees
 */
string BinomialHeapPQueue::extractMin() {
    if(logSize < 1) error("Queue is empty.");
    int minDegree = findMinDegree();
    string minValue = heapTrees[minDegree]->elem;
    BinomialHeapPQueue * tmp = new BinomialHeapPQueue(); // a heap that will contain the child trees of the tree whose root is min
    for(int i = 0; i < minDegree; i++) {
        tmp->heapTrees[i] = heapTrees[minDegree]->children[i]; // assign each child to the tmp heap
    }
    heapTrees.remove(minDegree); // deletes the min tree from this heap
    mergeBIntoA(*this, *tmp); // merges the temporary heap into this heap
    logSize --;
    delete tmp;
    return minValue;
}

/* enqueues a new element by creating a temporary heap with one element and merging it into this heap and deleting it
 */
void BinomialHeapPQueue::enqueue(const string& value) {
    logSize ++;
    BinomialHeapPQueue tmp;
    node * n = new node();
    n->elem = value;
    tmp.heapTrees[0] = n;
    mergeBIntoA(*this, tmp);
}

/* merges b into a and returns it deleting b
 */
BinomialHeapPQueue *BinomialHeapPQueue::merge(BinomialHeapPQueue * a, BinomialHeapPQueue * b) {
    a->mergeBIntoA(*a, *b);
    a->logSize += b->logSize;
    delete b;
    return a;
}

/* merges b into a by looping over remaining trees in b and adding them recursively to a
 */
void BinomialHeapPQueue::mergeBIntoA(BinomialHeapPQueue &a, BinomialHeapPQueue &b) {
    while(!b.heapTrees.isEmpty()) {
        int currentDegree = b.heapTrees.front();
        addTreesRecursive(currentDegree, a, b, nullptr);
    }
}

/* adds b to a recursively moving in further degrees until there's no more carry and both a and b are null at this degree
 */
void BinomialHeapPQueue::addTreesRecursive(int degree, BinomialHeapPQueue &a, BinomialHeapPQueue &b, node *carry) {
    int productDegree = -1; // the degree of the product of tree addition of the current iteration
    node * product = nullptr; // the tree product of the addition of the current iteration
    if(carry != nullptr) productDegree = mergeTrees(product, carry);
    if(b.heapTrees.containsKey(degree)) { productDegree = mergeTrees(product, b.heapTrees[degree]); b.heapTrees.remove(degree); }
    if(productDegree > degree) { addTreesRecursive(productDegree, a, b, product); return; } // keeps 'a' of this degree as it is
    if(a.heapTrees.containsKey(degree)) { productDegree = mergeTrees(product, a.heapTrees[degree]); a.heapTrees.remove(degree); }
    if(productDegree > degree) { addTreesRecursive(productDegree, a, b, product); return; } // keeps 'a' null and move further
    if(productDegree == degree) a.heapTrees[degree] = product; // reassign the tree to this degree of 'a'
}

/* attaches tree a to tree b or vice versa, according to the rules of binomial heap, storing the result in a, and returning result degree
 */
int BinomialHeapPQueue::mergeTrees(node *&a, node *&b) {
    if(a == nullptr) a = b;
    else {
        if(a->elem <= b->elem) a->children.add(b);  // attach b to a
        else { b->children.add(a); a = b; }         // attach a to b then assign a to equal b
    }
    return (a == nullptr) ? -1 : a->children.size(); // tree degree is the number of its children
}

/* returns the degree of the tree whose root is the smallest in the heap
 */
int BinomialHeapPQueue::findMinDegree() const {
    int minDegree = heapTrees.front();
    for(int i: heapTrees) {
        if(heapTrees[i]->elem < heapTrees[minDegree]->elem) minDegree = i;
    }
    return minDegree;
}
