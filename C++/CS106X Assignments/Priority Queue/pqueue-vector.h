#pragma once
#include "pqueue.h"
#include <string>
#include <vector.h>
#include <error.h>

class VectorPQueue : public PQueue {
public:
	VectorPQueue();
	~VectorPQueue();
	
	static VectorPQueue *merge(VectorPQueue *one, VectorPQueue *two);
	
	void enqueue(const std::string& elem);
    std::string extractMin();
    const std::string& peek() const;
	
private:
    Vector<std::string> vec; // stores the contents of the queue by the order of their addition
    int findMinIndex() const; // returns the index of the minimum value in the queue (in the vector vec); must be called when vec.size() > 0
};
