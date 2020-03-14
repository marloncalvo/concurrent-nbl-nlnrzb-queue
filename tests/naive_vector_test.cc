#include <iostream>
#include "naive_vector.h"

int main(void) {
    vector<int> v(1);
    int n_elems = 100;
    for (int i = 0; i < n_elems; i++) {
        v.push_back(i);
    }
    for (int i = 0; i < n_elems; i++) {
        std::cout << v.pop_back() << " ";
    }
}