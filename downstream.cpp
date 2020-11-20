#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "location_list.h"
#include "event_list.h"

void parse_data_file(void);
void parse_line(char* buf);
void parse_as_event(char* buf);
void parse_as_location(char* buf);

location_list* locations;
event_list* events;

int main(int argc, char* argv[]){
	locations = location_list_create();
	events = event_list_create();

	parse_data_file();

	printf("Data file parsed\n");

	location_list_print(locations);
	event_list_print(events);

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
	event_list_add(events, client_name, origin, destination, departure_time, arrival_time);
	printf("Event list added. Length = %d\n", event_list_size(events));
}

void parse_as_location(char* buf){
	int x;
	int y;
	char name[100];
	char* token;

	token = strtok(buf, " ");

	strcpy(name, token);
	token = strtok(NULL, " ");
	x = atoi(token);
	token = strtok(NULL, " ");
	y = atoi(token);
	location_list_add(locations, name, x, y);
}
