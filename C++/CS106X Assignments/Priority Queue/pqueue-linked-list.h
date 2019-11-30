#pragma once
#include "pqueue.h"
#include <string>
#include <error.h>

class LinkedListPQueue : public PQueue {
public:
	LinkedListPQueue();
	~LinkedListPQueue();
	
	static LinkedListPQueue *merge(LinkedListPQueue *one, LinkedListPQueue *two);
	
	void enqueue(const std::string& elem);
    std::string extractMin();
    const std::string& peek() const;
	
private:
    struct node {
        std::string value;
        node * sisters [2]; // sisters[0]: prev; sisters[1]: next; will make life easier later on
    };
    node * head;
    node * tail;
    node * lastElemStored;
};
