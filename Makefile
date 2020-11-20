CC = g++
CFLAGS = -g -Wall
LDFLAGS = -lpthread

all: downstream

downstream: downstream.o
	$(CC) $(CFLAGS) downstream.o -o downstream $(LDFLAGS)

downstream.o: downstream.cpp
	$(CC) $(CFLAGS) -c downstream.cpp

clean:
	rm -f *~ *.o downstream core *.tar *.zip *.gzip *.bzip *.gz

