CC = g++
CFLAGS = -g -Wall
LDFLAGS = -lssl -lcrypto -lpthread

all: main test

main: main.o parser.o spyglass.o settings.o
	$(CC) $(CFLAGS) parser.o spyglass.o main.o settings.o -o parser $(LDFLAGS)

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

test: spyglass_tester

spyglass_tester: spyglass.o spyglass_tester.o
	$(CC) $(CFLAGS) spyglass_tester.o spyglass.o -o spyglass_tester $(LDFLAGS)

spyglass_tester.o: spyglass_tester.cpp
	$(CC) $(CFLAGS) -c spyglass_tester.cpp

parser: parser.o settings.o
	$(CC) $(CFLAGS) parser.o settings.o -o parser $(LDFLAGS)

parser.o: parser.cpp
	$(CC) $(CFLAGS) -c parser.cpp

settings.o: settings.cpp
	$(CC) $(CFLAGS) -c settings.cpp

spyglass.o: spyglass.cpp
	$(CC) $(CFLAGS) -c spyglass.cpp

clean:
	rm -f *~ *.o parser *.lp
