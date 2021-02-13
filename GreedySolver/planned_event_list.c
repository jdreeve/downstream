#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "planned_event_list.h"

planned_event_list* planned_event_list_create(){
    planned_event_list* list = (planned_event_list*)malloc(sizeof(planned_event_list));
    if (list != NULL){
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    }//if
    return list;
}//planned_event_list_create

void planned_event_list_destroy(planned_event_list* list){
    /* How about freeing the list elements? */
    if (list != NULL && list->head != NULL){
            planned_event_node* current_planned_event_node = list->head;
            while(current_planned_event_node != NULL){
		    planned_event_node_delete(list, current_planned_event_node);
		    current_planned_event_node = list->head;
        }//while
    }//if
    /* Free listueue structure */
    if(list!= NULL){
    free(list);
    }//if
}//list_free

bool planned_event_list_add(planned_event_list* list, location_list* locations, char* client_name,int vehicle_number, char* origin, char* destination, int departure_time, int arrival_time){
    planned_event_node *new_planned_event_node;
        if(list==NULL){
    return false;
        }
    new_planned_event_node = (planned_event_node*)malloc(sizeof(planned_event_node));
        if(new_planned_event_node==NULL){
    return false;
        }
    new_planned_event_node->client_name = (char*)malloc((strlen(client_name)+1) * sizeof(char));
    
    strcpy(new_planned_event_node->client_name, client_name);
    new_planned_event_node->vehicle_number = vehicle_number;
    new_planned_event_node->departure_time = departure_time;
    new_planned_event_node->arrival_time = arrival_time;
    new_planned_event_node->origin = location_list_search(locations, origin);
    new_planned_event_node->destination = location_list_search(locations, destination);
    new_planned_event_node->next = list->head;

    list->head = new_planned_event_node;
    if (list->size==0){
        list->tail = new_planned_event_node;
    }//if
    else{
        new_planned_event_node->next->previous = new_planned_event_node;
    }
    list->size++;
        return true;
}

bool planned_event_node_delete(planned_event_list* list, planned_event_node* planned_event_node){
    if (list == NULL || planned_event_node == NULL){
        return false;
    }//if
    if (list->head==planned_event_node){//if planned_event_node is head
        list->head = list->head->next;
	if (planned_event_node->next != NULL){//if there are others
	    planned_event_node->next->previous = NULL;
	}//if
    }//if
    else if (list->tail==planned_event_node){//if planned_event_node is tail
        list->tail = list->tail->previous;
	if (planned_event_node->previous != NULL){//if there are others
	    planned_event_node->previous->next = NULL;
	}//if
    }//if
    else {//if planned_event_node is mid-list
        planned_event_node->previous->next = planned_event_node->next;
	planned_event_node->next->previous = planned_event_node->previous;
    }//else
    free(planned_event_node->client_name);
    free(planned_event_node);
    list->size--;
    return true;
}//planned_event_list_delete

void planned_event_list_print(planned_event_list* list){
	printf("Printing planned_event list\n");
	fflush(stdout);
	planned_event_node* iterator = list->head;
	while(iterator != NULL){
		printf("Vehicle: %d Origin: %s destination: %s departure: %d arrival: %d\n", iterator->vehicle_number, iterator->origin->name, iterator->destination->name, iterator->departure_time, iterator->arrival_time);
		iterator = iterator->next;
	}
}

int planned_event_list_size(planned_event_list* list){
    if (list==NULL){
        return 0;
    }
    return list->size;
}//planned_event_list_size

planned_event_node* planned_event_list_find_next_departure(planned_event_list* list, int current_time){
    if (list==NULL || list->size == 0){
        return NULL;
}
    planned_event_node* iterator = list->head;
    planned_event_node* next_departure = list->head;
    int least_next_departure_time = list->head->departure_time;

    while( (iterator != NULL)){
        if( (iterator->departure_time > current_time) && (iterator->departure_time < least_next_departure_time)){
		next_departure = iterator;
		least_next_departure_time = next_departure->departure_time;
	}
        iterator = iterator->next;
    }//while
    return next_departure;
}

planned_event_node* planned_event_list_get(planned_event_list* list, int index){
    if (list==NULL || list->size == 0|| list->size <= index){
        return NULL;
    }//if
    planned_event_node* iterator = list->head;
    for(int i=0; i<(index); i++){
        iterator = iterator->next;
    }
    return iterator;
}//planned_event_list_get
