CC = g++
CFLAGS = -g -Wall
LDFLAGS = -lpthread

all: downstream cbc_image

downstream: downstream.o location_list.o event_list.o vehicle_list.o greedy_solver.o planned_event_list.o
	$(CC) $(CFLAGS) downstream.o location_list.o event_list.o planned_event_list.o vehicle_list.o greedy_solver.o -o downstream $(LDFLAGS)

downstream.o: downstream.cpp
	$(CC) $(CFLAGS) -c downstream.cpp

location_list.o: location_list.c location_list.h
	$(CC) $(CFLAGS) -c location_list.c

event_list.o: event_list.c event_list.h
	$(CC) $(CFLAGS) -c event_list.c

planned_event_list.o: planned_event_list.c planned_event_list.h
	$(CC) $(CFLAGS) -c planned_event_list.c

vehicle_list.o: vehicle_list.c vehicle_list.h
	$(CC) $(CFLAGS) -c vehicle_list.c

greedy_solver.o: greedy_solver.c greedy_solver.h
	$(CC) $(CFLAGS) -c greedy_solver.c

cbc_image:
	docker build -t coin-or_cbc_ready .

clean:
	rm -f *~ *.o downstream core *.tar *.zip *.gzip *.bzip *.gz

