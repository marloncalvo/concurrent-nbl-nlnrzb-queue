#include <algorithm>

template<typename T>
class vector {

    private:
        T* array;
        size_t max_size;
        size_t tail_index;

    public:

        vector(size_t size) {
            max_size = size;
            tail_index = 0;
            array = (T*)malloc(sizeof(T)*max_size);
        }

        ~vector() {
            free(array);
        }

        void push_back(T item) {
            if (tail_index == max_size) reserve(max_size * 2);
            array[tail_index++] = item;
        }

        T pop_back() {
            if (tail_index == 0) throw "vector empty";
            return array[--tail_index];
        }

        T read(size_t index) {
            if (index < 0 || index >= tail_index) throw "index out of bounds"; 
            return array[index];
        }

        void write(size_t index, T item) {
            if(index < 0 || index >= tail_index) throw "index out of bounds";
            array[index] = item;
        }

        size_t size() {
            return tail_index;
        }

        void reserve(size_t size) {
            T* n_array = (T*)malloc(sizeof(T)*size);
            std::copy_n(array, max_size, n_array);
            max_size = size;
            free(array);
            array = n_array;
        }
};


