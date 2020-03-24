/**
 * Below you will find the implementation for the MRLock QQueue.
 * The class defines all the methods we support, and the implementation is there as well.
 * This holds the same correctness and progress conditions as the original QStack.
 * We chose to avoid forking here, as locking introduces sequentially inherently,
 * but much more useful for try-ing implementations.
 */

#include <vector>
#include <algorithm>
#include <iostream>
#include <atomic>
#include "mrlock.h"
#include "bitset.h"

#define MAX_CHILD 1

#define PUSH 0
#define POP 1
#define HEAD 2
#define TAIL 3

#define LOCK_SIZE 2

/**
 * This is placed in unfinished pops, so that another thread knows
 * how to return a pushed value.
 */
template<typename T>
class Descriptor {
public:
	int op;
	T *adr;
};

template<typename T>
class Node {
public:
	Node<T> *prev;
	Node<T> *nexts[MAX_CHILD];
	int op;
	T val;
	Descriptor<T> *desc;
};

template<typename T>
class QQueue {
private:
	Node<T> *head;
	Node<T> *tail;
	MRLock<Bitset> *mrlock;
	
	Node<T>* remove(Node<T> *cur);
	void insert(Node<T> *tail, Node<T> *elem);

public:
	QQueue();
	void push(T val);
	void pop(T *adr);
};

/**
 * Mostly like the single-threaded version, except we are now locking
 * head and tail, so that we can insert or remove a node, where remove nodes
 * are unfinished pops.
 */
template<typename T>
void QQueue<T>::push(T val) {

	// We need both since head and tail may be next to each other.
	// Furthermore, we can only grab one or both at once, so we decided
	// to grab both to avoid deadlocking (with performance costs, as we cause
	// queuing for both push and pop operations).
	Bitset *res = new Bitset();
	res->Resize(LOCK_SIZE);
	res->Set(TAIL);
	res->Set(HEAD);

	int pos = mrlock->Lock(*res);

	// Check if tail contains a unfinished POP. If so, finish it.
	// Else just insert.
	if(tail->op != POP) {
		Node<T> *node = new Node<T>();
		node->op = PUSH;
		node->val = val;
		insert(tail, node);
	} else {
		Node<T> *elem = remove(head);
		*(elem->desc->adr) = val;
		if(head->nexts[0] == NULL) {
			tail = head;
		}
	}

	mrlock->Unlock(pos);
}

template<typename T>
void QQueue<T>::pop(T *adr) {

	// We require both, since head and tail can be next to each other.
	// Also, we may be updating tail or head, and we cannot grab one
	// after we grabbed another end (avoiding deadlock).
	Bitset *res = new Bitset();
	res->Resize(LOCK_SIZE);
	res->Set(TAIL);
	res->Set(HEAD);

	int pos = mrlock->Lock(*res);

	// We need to check if it's an invalid pop first.
	// If it is, we create a new Node with a descriptor, and insert
	// into queue. Else, we just pop like normal.
	if (head == tail || tail->op != PUSH) {
		Node<T> *node = new Node<T>();
		Descriptor<T> *desc = new Descriptor<T>();

		desc->adr = adr;
		desc->op  = POP;

		node->op = POP;
		node->desc = desc;
		
		insert(tail, node);

	// Just pop and go.
	} else {
		Node<T> *elem = remove(head);
		*adr = elem->val;
		if(head->nexts[0] == NULL) {
			tail = head;
		}
	}

	mrlock->Unlock(pos);
}

/*
 * Since we are locked before calling this method,
 * we dont need to worry about synchronization.
 * We just need to update tail.
 */
template<typename T>
void QQueue<T>::insert(Node<T> *cur, Node<T> *elem) {
	cur->nexts[0] = elem;
	elem->prev = cur;

	tail = elem;
}

/**
 * Since we are locked before calling this method, we dont really
 * need to worry about synchronization.
 */
template<typename T>
Node<T>* QQueue<T>::remove(Node<T> *cur) {
	Node<T> *elem = cur->nexts[0];

	if(elem->nexts[0] != NULL) {
		elem->nexts[0]->prev = cur;
	}

	cur->nexts[0] = elem->nexts[0];

	return elem;
}

template<typename T>
QQueue<T>::QQueue() {
	head = new Node<T>();
	tail = head;
	mrlock = new MRLock<Bitset>(100); // 100 is good enough for this.
}