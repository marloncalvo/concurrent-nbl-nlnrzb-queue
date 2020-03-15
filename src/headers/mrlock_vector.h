#include <atomic>
#include <algorithm>

#define DEFAULT_SIZE 1

template<typename T>
class vector {
private:
	std::atomic<size_t> max_size;
	std::atomic<size_t> indx;
	T *arr;

public:
	vector();
	~vector();
	void push_back(T item);
	void write(T item, size_t index);
	T pop_back();
	T read(size_t index);
	size_t size();
	void reserve(size_t n_size);
};

template<typename T>
void vector<T>::push_back(T item) {
	
}

template<typename T>
vector<T>::vector(): max_size(DEFAULT_SIZE), indx(5) {
	arr = (T*)malloc(sizeof(T) * DEFAULT_SIZE);
}

template<typename T>
vector<T>::~vector() {
	free(arr);
}

