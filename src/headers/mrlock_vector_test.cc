#include <iostream>

#include "mrlock_vector.h"

#define N 1

int main(void) {

	vector<int> vector(N);

	for (int i = 0; i < N; i++) {
		std::cout << "Pushing " << i << std::endl;
		vector.push_back(i);
		std::cout << "Pushed " << i << std::endl;
	}

	return 0;
}