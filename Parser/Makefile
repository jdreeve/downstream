CC = g++
CFLAGS = -g -Wall
LDFLAGS = -lssl -lcrypto -lpthread -lortools

all: parser_exec scheduler_exec

parser_exec: parser parser_exec.o
	$(CC) $(CFLAGS) parser_exec.o settings.o spyglass.o -o parser $(LDFLAGS)

scheduler_exec: schedule_writer schedule_exec.o
	$(CC) $(CFLAGS) schedule_exec.o schedule_writer.o settings.o spyglass.o -o scheduler $(LDFLAGS)

schedule_writer: schedule_writer.o settings.o;

parser: settings.o spyglass.o; 

parser_exec.o: parser_exec.cpp
	$(CC) $(CFLAGS) -c parser_exec.cpp

schedule_exec.o: schedule_exec.cpp
	$(CC) $(CFLAGS) -c schedule_exec.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

schedule_writer.o: schedule_writer.cpp
	$(CC) $(CFLAGS) -c schedule_writer.cpp

settings.o: settings.cpp
	$(CC) $(CFLAGS) -c settings.cpp

spyglass.o: spyglass.cpp
	$(CC) $(CFLAGS) -c spyglass.cpp

clean:
	rm -f *~ *.o parser scheduler spyglass_tester *.lp
