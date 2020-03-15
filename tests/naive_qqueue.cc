#include <iostream>
#include "naive_qqueue.h"

#define N 100

int main(void) {
	int *deqs = (int*)malloc(sizeof(int)*(N+1));
	QQueue<int> q;
	for (int i = 0; i < N; i++) {
		q.push(i);
	}
	for (int i = 0; i < N+1; i++) {
		q.pop(deqs+i);
	}

	for(int i = 0; i < N+1; i++) {
		std::cout << deqs[i] << std::endl;
	}

	// Notice that we pushed N times, but popped N + 1 times.
	// Verify will be less than 0, which will fail the quantifiability test.
	// After we push, verify will be zero, and the address at deqs[N] will be updated.

	std::cout << "Verify Val = " << q.verify() << std::endl;

	q.push(123);

	std::cout << deqs[N] << std::endl;

	std::cout << "Verify Val = " << q.verify() << std::endl;
}