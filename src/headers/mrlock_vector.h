#include <atomic>
#include <algorithm>
#include <stdint.h>
#include "mrlock.h"

#define DEFAULT_SIZE 1
#define TAIL_LOCK 0

template<typename T>
class vector {
private:
	MRLock<Bitset> *lock;
	Bitset * resize_res;
	std::atomic<uint32_t> cur_index;
	volatile uint32_t max_size;
	volatile T *arr;

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


	Bitset * res; uint32_t index; uint32_t pos = -1;
	do {
		if (pos != -1) {
			lock->Unlock(pos);
		}
		
		res = new Bitset();
		res->Resize(index+2);
		res->Set(index+1);
		res->Set(0);
		pos = lock->Lock(*res);
	} while(!cur_index.compare_exchange_strong(index, index+1));

	arr[index] = item;
	lock->Unlock(pos);
	delete res;
}

template<typename T>
T vector<T>::pop_back() {
	Bitset * res; uint32_t index; uint32_t pos = -1;
	do {
		if (pos != -1) {
			lock->Unlock(pos);
		}
		index = cur_index.load();
		res = new Bitset();
		res->Resize(index+2);
		res->Set(index+1);
		res->Set(0);
		pos = lock->Lock(*res);
	} while(!cur_index.compare_exchange_strong(index, index-1));

	T item = arr[index-1];
	lock->Unlock(pos);
	delete res;
	return item;
}

template<typename T>
void vector<T>::write(T item, uint32_t index) {
	Bitset * res = new Bitset();
	res->Resize(index+2);
	res->Set(index+1);
	res->Set(0);
	uint32_t pos = lock->Lock(*res);
	arr[index] = item;
	lock->Unlock(pos);
	delete res;
}

template<typename T>
T vector<T>::read(uint32_t index) {
	Bitset * res = new Bitset();
	res->Resize(index+2);
	res->Set(index+1);
	res->Set(0);
	uint32_t pos = lock->Lock(*res);
	T item = arr[index];
	lock->Unlock(pos);
	return item;
}

template<typename T>
void vector<T>::reserve(uint32_t size) {
	uint32_t pos = lock->Lock(*resize_res);

	uint32_t p_max_size = max_size.exchange(size);
	T* n_array = (T*)malloc(sizeof(T)*size);
    std::copy_n(arr, p_max_size, n_array);
   	free(arr);
    arr = n_array;

    lock->Unlock(pos);
}

template<typename T>
uint32_t vector<T>::size() {
	return cur_index.load();
}

template<typename T>
vector<T>::vector(): max_size(DEFAULT_SIZE), cur_index(0) {
	arr = (T*)malloc(sizeof(T) * DEFAULT_SIZE);
	lock = new MRLock<Bitset>(1);
	resize_res = new Bitset();
	resize_res->Resize(1);
	resize_res->Set(0);
}

template<typename T>
vector<T>::~vector() {
	delete[] arr;
	delete lock;
}

