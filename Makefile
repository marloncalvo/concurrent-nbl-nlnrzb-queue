CXX = g++
CFLAGS = -g -Wall

HEADERS = src/headers

TARGET = mrlock_qqueue.cc

all: clean mrlock_qqueue qqueue

naive_qqueue: clean
	$(CXX) -O3 -Wall -pthread -I $(HEADERS) -g src/tests/naive_qqueue.cc -o naive_test

mrlock_qqueue: clean
	$(CXX) -O3 -Wall -pthread -I $(HEADERS) -g src/tests/mrlock_qqueue.cc -o mrlock_test

qqueue:
	javac src/**/*.java

clean:
	$(RM) $(TARGET)
	$(RM) src/**/*.class