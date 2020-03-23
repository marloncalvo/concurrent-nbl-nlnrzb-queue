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

template<typename T>
void QQueue<T>::push(T val) {

	Bitset *res = new Bitset();
	res->Resize(LOCK_SIZE);
	res->Set(TAIL);
	res->Set(HEAD);

	int pos = mrlock->Lock(*res);

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

	Bitset *res = new Bitset();
	res->Resize(LOCK_SIZE);
	res->Set(TAIL);
	res->Set(HEAD);

	int pos = mrlock->Lock(*res);

	if (head == tail || tail->op != PUSH) {
		Node<T> *node = new Node<T>();
		Descriptor<T> *desc = new Descriptor<T>();

		desc->adr = adr;
		desc->op  = POP;

		node->op = POP;
		node->desc = desc;
		
		insert(tail, node);

	} else {
		Node<T> *elem = remove(head);
		*adr = elem->val;
		if(head->nexts[0] == NULL) {
			tail = head;
		}
	}

	mrlock->Unlock(pos);
}

template<typename T>
void QQueue<T>::insert(Node<T> *cur, Node<T> *elem) {
	cur->nexts[0] = elem;
	elem->prev = cur;

	tail = elem;
}

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
	mrlock = new MRLock<Bitset>(100);
}