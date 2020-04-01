CXX = g++
CFLAGS = -g -Wall

HEADERS = src/headers

TARGET = mrlock_qqueue.cc

all: clean mrlock_qqueue qqueue

mrlock_qqueue: clean
	$(CXX) -pthread -I $(HEADERS) -g tests/mrlock_qqueue.cc -o mrlock_test

qqueue: clean
	javac src/qqueue/*.java

clean:
	$(RM) $(TARGET)