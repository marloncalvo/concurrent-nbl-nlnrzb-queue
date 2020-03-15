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

template<typename T>
void QQueue<T>::push(T val) {
	Node<T> * node = new Node<T>();
	node->val = val;
	node->next = tail;
	node->prev = tail->prev;
	tail->prev->next = node;
	tail->prev = node;

	if(!failed_pops.empty()) {
		T *adr = failed_pops.top();
		
		std::cout << "Address = " << *adr << std::endl;
		*adr = val;
		std::cout << "Address = " << *adr << std::endl;

		failed_pops.pop();
	}
}

template<typename T>
void QQueue<T>::pop(T *adr) {
	if (head->next != tail) {
		Node<T> * tmp = head->next;
		tmp->next->prev = head;
		head->next = tmp->next;
		*adr = tmp->val;	
		std::cout << "Popped " << tmp->val << std::endl;
		delete tmp;
	} else {
		failed_pops.push(adr);
	}
}

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