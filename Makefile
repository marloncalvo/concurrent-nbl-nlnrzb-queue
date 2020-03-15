CXX = g++
CFLAGS = -g -Wall

HEADERS = src/headers

TARGET = naive_qqueue.cc mrlock_qqueue.cc

all: clean naive_qqueue mrlock_qqueue

naive_qqueue: clean
	$(CXX) -pthread -I $(HEADERS) -g tests/naive_qqueue.cc -o naive_test

mrlock_qqueue: clean
	$(CXX) -pthread -I $(HEADERS) -g tests/mrlock_qqueue.cc -o mrlock_test

clean:
	$(RM) $(TARGET)

