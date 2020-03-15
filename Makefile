CXX = g++
CFLAGS = -g -Wall

HEADERS = src/headers

TARGET = naive_vector mrlock_vector

all: clean naive_test mrlock_test

naive_test: clean
	$(CXX) -pthread -I $(HEADERS) -g tests/naive_vector_test.cc -o naive_test

mrlock_test: clean
	$(CXX) -pthread -I $(HEADERS) -g tests/mrlock_vector_test.cc -o mrlock_test

clean:
	$(RM) $(TARGET)

