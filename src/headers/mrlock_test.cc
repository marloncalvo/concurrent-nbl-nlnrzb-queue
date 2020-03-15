#include <iostream>
#include <bitset>
#include <atomic>
#include <thread>

#include "mrlock.h"
#include "bitset.h"

MRLock<Bitset> mrlock(8);

void lock(int id) {

	/*
	Bitset * a_res = new Bitset();
	a_res->Resize(8);

	mrlock.Lock(*a_res);
	*/
	std::cout << "Acquired Lock: " << id << std::endl;
}

int main(void) {
	
	std::thread t1(lock, 0);
	std::thread t2(lock, 1);	
}