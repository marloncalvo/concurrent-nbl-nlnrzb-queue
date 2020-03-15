#include <iostream>
#include <bitset>
#include <atomic>
#include <thread>
#include <string>

#include "mrlock.h"
#include "bitset.h"

MRLock<Bitset> mrlock(100);

void lock(int id) {

	
	Bitset * a_res = new Bitset();
	a_res->Resize(10);

	a_res->Set(100);

	uint32_t pos = mrlock.Lock(*a_res);
	
	std::cout << "Acquired Lock: " + std::to_string(id) + "\n";

	mrlock.Unlock(pos);
}

int main(void) {
	
	std::thread t1(lock, 0);
	std::thread t2(lock, 1);

	t1.join();
	t2.join();
}