#include <atomic>
#include <algorithm>
#include <stdint.h>

#include "mrlock.h"

#define DEFAULT_SIZE 1

template<typename T>
class vector {
private:
	MRLock<Bitset> lock;
	std::atomic<uint32_t> max_size;
	std::atomic<uint32_t> indx;
	T *arr;

public:
	vector();
	vector(uint32_t size);
	~vector();
	void push_back(T item);
	void write(T item, uint32_t index);
	T pop_back();
	T read(uint32_t index);
	uint32_t size();
	void reserve(uint32_t n_size);
};

template<typename T>
void vector<T>::push_back(T item) {
	MRLock<Bitset> test(100);
	Bitset * res = new Bitset();
	uint32_t index = indx.load();

	res->Resize(UINT32_MAX);
	res->Set(index);

	std::cout << "Acquiring Lock" << std::endl;
	uint32_t pos = test.Lock(*res);
	std::cout << pos << std::endl;
	std::cout << "Acquired Lock" << std::endl;
	arr[index] = item;
	indx.store(index+1);
	std::cout << "Stored." << std::endl;
	test.Unlock(pos);
	std::cout << "Done." << std::endl;
}

template<typename T>
vector<T>::vector(uint32_t size): max_size(size), indx(0), lock(UINT32_MAX) {
	arr = (T*)malloc(sizeof(T) * size);
}

template<typename T>
vector<T>::vector(): max_size(DEFAULT_SIZE), indx(0) {
	arr = (T*)malloc(sizeof(T) * DEFAULT_SIZE);
}

template<typename T>
vector<T>::~vector() {
	free(arr);
}

