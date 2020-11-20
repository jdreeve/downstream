CC = g++
CFLAGS = -g -Wall
LDFLAGS = -lpthread

all: downstream

downstream: downstream.o location_list.o event_list.o
	$(CC) $(CFLAGS) downstream.o location_list.o event_list.o -o downstream $(LDFLAGS)

downstream.o: downstream.cpp
	$(CC) $(CFLAGS) -c downstream.cpp

location_list.o: location_list.c location_list.h
	$(CC) $(CFLAGS) -c location_list.c

event_list.o: event_list.c event_list.h
	$(CC) $(CFLAGS) -c event_list.c

clean:
	rm -f *~ *.o downstream core *.tar *.zip *.gzip *.bzip *.gz

