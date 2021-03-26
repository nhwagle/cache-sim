CC = g++
CONSERVATIVE_FLAGS = -std=c++11 -Wall -Wextra -pedantic
DEBUGGING_FLAGS = -g -O0
CFLAGS = $(CONSERVATIVE_FLAGS) $(DEBUGGING_FLAGS)

main.o: main.cpp functions.h
	$(CC) -c -o $@ $< $(CFLAGS)

csim: main.o functions.o
	$(CC) $(CFLAGS) -o csim main.o functions.o

functions.o: functions.cpp functions.h
	$(CC) $(CFLAGS) -c functions.cpp

clean:
	rm -f *.o csim