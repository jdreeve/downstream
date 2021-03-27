CC = g++
CFLAGS = -g -Wall
LDFLAGS = -lssl -lcrypto

all: main test

main: main.o parser.o spyglass.o
	$(CC) $(CFLAGS) parser.o spyglass.o main.o -o parser $(LDFLAGS)

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

test: spyglass_tester

spyglass_tester: spyglass.o spyglass_tester.o
	$(CC) $(CFLAGS) spyglass_tester.o spyglass.o -o spyglass_tester $(LDFLAGS)

spyglass_tester.o: spyglass_tester.cpp
	$(CC) $(CFLAGS) -c spyglass_tester.cpp

parser: parser.o 
	$(CC) $(CFLAGS) parser.o -o parser $(LDFLAGS)

parser.o: parser.cpp
	$(CC) $(CFLAGS) -c parser.cpp

spyglass.o: spyglass.cpp
	$(CC) $(CFLAGS) -c spyglass.cpp

clean:
	rm -f *~ *.o parser *.lp
