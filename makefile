CC=g++
CFLAGS=-c -Wall
LDFLAGS=-mconsole
OBJ=long_numbers.o tests.o pi.o

all: long_numbers tests pi

long_numbers: long_numbers.o
	$(CC) $(LDFLAGS) long_numbers.o -o long_numbers

tests: long_numbers.o tests.o
	$(CC) $(LDFLAGS) long_numbers.o tests.o -o tests

pi: long_numbers.o pi.o
	$(CC) $(LDFLAGS) long_numbers.o pi.o -o pi

long_numbers.o: long_numbers.cpp long_numbers.hpp
	$(CC) $(CFLAGS) long_numbers.cpp

tests.o: tests.cpp long_numbers.hpp
	$(CC) $(CFLAGS) tests.cpp

pi.o: pi.cpp long_numbers.hpp
	$(CC) $(CFLAGS) pi.cpp

test: tests
	./tests

pi_run: pi
	./pi

clean:
	rm -rf *.o long_numbers tests pi
