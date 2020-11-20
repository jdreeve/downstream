#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "location_list.h"
#include "event_list.h"
#include "vehicle_list.h"
#include "greedy_solver.h"

#define DEBUG 1

void parse_data_file(void);
void parse_line(char* buf);
void parse_as_event(char* buf);
void parse_as_location(char* buf);
void parse_as_vehicle(char* buf);

event_list* events;
vehicle_list* vehicles;
location_list* locations;

int main(int argc, char* argv[]){
	events = event_list_create();
	locations = location_list_create();
	vehicles = vehicle_list_create(NULL);
	if(DEBUG){
	printf("Parsing data file...\n");
	fflush(stdout);
	}
	parse_data_file();
	char homestring[10] = "home";
	location_node* home = location_list_search(locations, homestring);
	vehicle_list_set_home(vehicles, home);
	if(home==NULL){
		printf("NULL0\n");
		fflush(stdout);
	}
	if(DEBUG){
	printf("Data file parsed\n");
	printf("Printing location list...\n");
	fflush(stdout);
	location_list_print(locations);
	printf("Location list printed\n");
	printf("Printing event list...\n");
	fflush(stdout);
	event_list_print(events);
	printf("Event list printed\n");
	printf("Printing vehicle list...\n");
	fflush(stdout);
	vehicle_list_print(vehicles);
	printf("Vehicle list printed\n");
	printf("Running GreedySolver\n");
	fflush(stdout);
	}
	run_greedy_solver(locations, events, vehicles);

	event_list_destroy(events);
	location_list_destroy(locations);
	printf("Main exiting\n");
}

void parse_data_file(void){
	FILE* data_file = fopen("data.txt", "r");
	char buf[300];
	char *index;

	while(fgets(buf, sizeof(buf), data_file) != NULL){
		if((index = strchr(buf, '#'))!= NULL){
			*index = '\0';
		}
		parse_line(buf);
	}
}

void parse_line(char* buf){
	char* index;
	if((index = strchr(buf, ' '))!= NULL){
		*index = '\0';
	}
	if(!strcmp(buf, "location")){
		*index = ' ';
		parse_as_location(index+1);
	}
	else if(!strcmp(buf, "event")){
		*index = ' ';
		parse_as_event(index+1);
	}
	else if(!strcmp(buf, "vehicle")){
		*index = ' ';
		parse_as_vehicle(index+1);
	}
}

void parse_as_event(char* buf){
	char client_name[50];
	char origin[50];
	char destination[50];
	int departure_time;
	int arrival_time;
	char* token;

	token = strtok(buf, "V ");
	strcpy(client_name, token);
	token = strtok(NULL, " ");
	strcpy(origin, token);
	token = strtok(NULL, " ");
	strcpy(destination, token);
	token = strtok(NULL, " ");
	departure_time = atoi(token);
	token = strtok(NULL, " ");
	arrival_time = atoi(token);
	event_list_add(events, locations, client_name, origin, destination, departure_time, arrival_time);
}

void parse_as_location(char* buf){
	int x;
	int y;
	char name[100];
	char* token;

	token = strtok(buf, " ");

	strcpy(name, token);
	printf("Name =\"%s\"\n", name);
	token = strtok(NULL, " ");
	x = atoi(token);
	token = strtok(NULL, " ");
	y = atoi(token);
	location_list_add(locations, name, x, y);
}


void parse_as_vehicle(char* buf){
	int capacity;
	char* token;

	token = strtok(buf, " ");
	capacity = atoi(token);
	vehicle_list_add(vehicles, capacity);
}
