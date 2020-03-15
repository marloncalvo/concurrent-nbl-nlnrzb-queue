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
	Bitset * res = new Bitset();
	uint32_t index = indx.load();

	res->Resize(100);
	res->Set(index);

	uint32_t pos = lock.Lock(*res);
	arr[index] = item;
	indx.store(index+1);
	lock.Unlock(pos);
}

template<typename T>
vector<T>::vector(uint32_t size): max_size(size), indx(0), lock(100) {
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

