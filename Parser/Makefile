CC = g++
CFLAGS = -g -Wall
LDFLAGS = -lssl -lcrypto -lpthread

all: parser_exec scheduler_exec test

parser_exec: parser parser_exec.o
	$(CC) $(CFLAGS) parser_exec.o parser.o settings.o spyglass.o -o parser $(LDFLAGS)

scheduler_exec: schedule_writer schedule_exec.o
	$(CC) $(CFLAGS) schedule_exec.o schedule_writer.o parser.o settings.o spyglass.o -o scheduler $(LDFLAGS)

schedule_writer: schedule_writer.o parser.o settings.o;

parser: parser.o settings.o spyglass.o; 

parser_exec.o: parser_exec.cpp
	$(CC) $(CFLAGS) -c parser_exec.cpp

schedule_exec.o: schedule_exec.cpp
	$(CC) $(CFLAGS) -c schedule_exec.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

test: spyglass_tester

spyglass_tester: spyglass.o spyglass_tester.o
	$(CC) $(CFLAGS) spyglass_tester.o spyglass.o -o spyglass_tester $(LDFLAGS)

schedule_writer.o: schedule_writer.cpp
	$(CC) $(CFLAGS) -c schedule_writer.cpp

spyglass_tester.o: spyglass_tester.cpp
	$(CC) $(CFLAGS) -c spyglass_tester.cpp

parser.o: parser.cpp
	$(CC) $(CFLAGS) -c parser.cpp

settings.o: settings.cpp
	$(CC) $(CFLAGS) -c settings.cpp

spyglass.o: spyglass.cpp
	$(CC) $(CFLAGS) -c spyglass.cpp

clean:
	rm -f *~ *.o parser scheduler spyglass_tester *.lp
