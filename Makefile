CXX = g++
CFLAGS = -g -Wall

HEADERS = src/headers

TARGET = mrlock_qqueue.cc

all: clean mrlock_qqueue

mrlock_qqueue: clean
	$(CXX) -pthread -I $(HEADERS) -g tests/mrlock_qqueue.cc -o mrlock_test

clean:
	$(RM) $(TARGET)

