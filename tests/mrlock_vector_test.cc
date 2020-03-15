#include <iostream>
#include "mrlock_vector.h"

#define N 10000																																																																																																																																			

MRLock<Bitset> * mrlock = new MRLock<Bitset>(1);

void lock(int id, int size) {

	Bitset * a_res = new Bitset();
	a_res->Resize(size);																																																																									
	a_res->Set(size);
	mrlock->Lock(*a_res);

	std::cout << "Acquired Lock: " << id << std::endl;
}

int main2(void) {

	std::thread t1(lock, 0, 50);
	std::thread t2(lock, 1, 100);

	t1.join();
	t2.join();

	return 0;
}

int main(void) {

	vector<int> vector;

	for (int i = 0; i < N; i++) {
		//std::cout << "Pushing " << i << std::endl;
		vector.push_back(i);
		std::cout << "Pushed " << i << std::endl;
	}

	for (int i = 0; i < N; i++) {
		//std::cout << "Pushing " << i << std::endl;
		std::cout << "Popped " << vector.pop_back() << std::endl;
	}

	return 0;
}