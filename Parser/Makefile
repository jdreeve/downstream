CC = g++
CFLAGS = -g -Wall
LDFLAGS = -lpthread

all: parser

parser: parser.o 
	$(CC) $(CFLAGS) parser.o -o parser $(LDFLAGS)

parser.o: parser.cpp
	$(CC) $(CFLAGS) -c parser.cpp

clean:
	rm -f *~ *.o parser *.lp
