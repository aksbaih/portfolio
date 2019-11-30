#pragma once
#include "pqueue.h"
#include <string>
#include <error.h>

class HeapPQueue : public PQueue {
public:
	HeapPQueue();
	~HeapPQueue();
	
	static HeapPQueue *merge(HeapPQueue *one, HeapPQueue *two);
	
	void enqueue(const std::string& elem);
    std::string extractMin();
    const std::string& peek() const;
    
private:
    std::string *array; // heap array
    int capacity; // how many elements the array can have
    void expandArray(int newSize);
    void heapify(int index);
    void heapifyParentsRecursive(int index);
    void heapifyLevelsRecursive(int level);
    void swap(std::string& a, std::string& b);
};
