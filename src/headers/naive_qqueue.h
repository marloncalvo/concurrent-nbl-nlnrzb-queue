#include <stack>
#include <algorithm>

template<typename T>
class Node {
public:
	Node * prev;
	Node * next;
	T val;
};

template<typename T>
class QQueue {
private:
	std::stack<T*> failed_pops;
	Node<T> *head;
	Node<T> *tail;
	bool find(T val);

public:
	QQueue();
	void push(T val);
	void pop(T *adr);
	int verify();
};

/**
 * This just acts like a simple queue, except it checks for failed pops in the 
 * failed_pops stack before pushing. If it finds anything, it populates that
 * address with the val passed into push.
 */
template<typename T>
void QQueue<T>::push(T val) {
	if(!failed_pops.empty()) {

		T *adr = failed_pops.top();
		*adr = val;
		failed_pops.pop();

	} else {

		Node<T> * node = new Node<T>();
		node->val = val;
		node->next = tail;
		node->prev = tail->prev;
		tail->prev->next = node;
		tail->prev = node;

	}
}

// This simply checks if we are at the tail.
// If it is the case that head->next is tail, that means there are
// no pushed elements currently.
template<typename T>
void QQueue<T>::pop(T *adr) {
	if (head->next != tail) {

		Node<T> * tmp = head->next;
		tmp->next->prev = head;
		head->next = tmp->next;
		*adr = tmp->val;	
		delete tmp;

	} else {
		failed_pops.push(adr);
	}
}

// Verification test to see if this run was quantifiable.
template<typename T>
int QQueue<T>::verify() {
	return failed_pops.size();
}

template<typename T>
QQueue<T>::QQueue() {
	head = new Node<T>();
	tail = new Node<T>();
	head->next = tail;
	tail->prev = head;
}