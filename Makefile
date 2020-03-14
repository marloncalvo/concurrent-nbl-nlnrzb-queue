CXX = g++
CFLAGS = -g -Wall

HEADERS = src/headers

TARGET = naive_vector_test

all: $(TARGET)

$(TARGET): tests/$(TARGET).cc
	$(CXX) $(CFLAGS) -o $(TARGET) tests/$(TARGET).cc -I $(HEADERS)

clean:
	$(RM) $(TARGET)

