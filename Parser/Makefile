CC = g++
CFLAGS = -g -Wall
LDFLAGS = -lpthread

all: parser

parser: parser.o location_list.o vehicle_list.o  
	$(CC) $(CFLAGS) parser.o location_list.o vehicle_list.o -o parser $(LDFLAGS)

parser.o: parser.cpp
	$(CC) $(CFLAGS) -c parser.cpp

location_list.o: location_list.c location_list.h
	$(CC) $(CFLAGS) -c location_list.c

vehicle_list.o: vehicle_list.c vehicle_list.h
	$(CC) $(CFLAGS) -c vehicle_list.c

clean:
	rm -f *~ *.o parser *.lp
