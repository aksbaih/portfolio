#pragma once
#include "pqueue.h"
#include <string>
#include <vector.h>
#include <map.h>
#include <set.h>
#include <algorithm>

class BinomialHeapPQueue : public PQueue {
public:
	BinomialHeapPQueue();
	~BinomialHeapPQueue();
	
	static BinomialHeapPQueue *merge(BinomialHeapPQueue *one, BinomialHeapPQueue *two);
	
	void enqueue(const std::string& elem);
    std::string extractMin();
    const std::string& peek() const;
	
private:
    struct node {
        std::string elem;
        Vector<node *> children;
        ~node() {
            while(!children.isEmpty()) delete children.back();
        }
    };
    Map<int, node *> heapTrees; // key: degree. value: tree root.
    void mergeBIntoA(BinomialHeapPQueue& A, BinomialHeapPQueue& B); // does not destroy A nor B
    void addTreesRecursive(int degree, BinomialHeapPQueue& a, BinomialHeapPQueue&b, node * carry);
    int mergeTrees(node* &a, node* &b);
    int findMinDegree() const;
};

