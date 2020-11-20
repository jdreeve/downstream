#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "event_list.h"

event_list* event_list_create(){
    event_list* list = (event_list*)malloc(sizeof(event_list));
    if (list != NULL){
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    }//if
    return list;
}//event_list_create

void event_list_destroy(event_list* list){
    /* How about freeing the list elements? */
    if (list != NULL && list->head != NULL){
            event_node* current_event_node = list->head;
            while(current_event_node != NULL){
		    event_node_delete(list, current_event_node);
		    current_event_node = list->head;
        }//while
    }//if
    /* Free listueue structure */
    if(list!= NULL){
    free(list);
    }//if
}//list_free

bool event_list_add(event_list* list, char* client_name, char* origin, char* destination, int departure_time, int arrival_time){
    event_node *new_event_node;
        if(list==NULL){
    return false;
        }
    new_event_node = (event_node*)malloc(sizeof(event_node));
        if(new_event_node==NULL){
    return false;
        }
    new_event_node->client_name = (char*)malloc((strlen(client_name)+1) * sizeof(char));
    new_event_node->origin = (char*)malloc((strlen(origin)+1) * sizeof(char));
    new_event_node->destination = (char*)malloc((strlen(destination)+1) * sizeof(char));
    
    strcpy(new_event_node->client_name, client_name);
    strcpy(new_event_node->origin, origin);
    strcpy(new_event_node->destination, destination);
    new_event_node->departure_time = departure_time;
    new_event_node->arrival_time = arrival_time;
    new_event_node->next = list->head;
    list->head = new_event_node;
    if (list->size==0){
        list->tail = new_event_node;
    }//if
    else{
        new_event_node->next->previous = new_event_node;
    }
    list->size++;
        return true;
}

bool event_node_delete(event_list* list, event_node* event_node){
    if (list == NULL || event_node == NULL){
        return false;
    }//if
    if (list->head==event_node){//if event_node is head
        list->head = list->head->next;
	if (event_node->next != NULL){//if there are others
	    event_node->next->previous = NULL;
	}//if
    }//if
    else if (list->tail==event_node){//if event_node is tail
        list->tail = list->tail->previous;
	if (event_node->previous != NULL){//if there are others
	    event_node->previous->next = NULL;
	}//if
    }//if
    else {//if event_node is mid-list
        event_node->previous->next = event_node->next;
	event_node->next->previous = event_node->previous;
    }//else
    free(event_node->client_name);
    free(event_node->origin);
    free(event_node->destination);
    free(event_node);
    list->size--;
    return true;
}//event_list_delete

void event_list_print(event_list* list){
	event_node* iterator = list->head;
	while(iterator != NULL){
		printf("origin: %s destination: %s departure: %d arrival: %d\n", iterator->origin, iterator->destination, iterator->departure_time, iterator->arrival_time);
		iterator = iterator->next;
	}
}

int event_list_size(event_list* list){
    if (list==NULL){
        return 0;
    }
    return list->size;
}//event_list_size

event_node* event_list_search(event_list* list, char* origin){
	printf("DEBUG: Search not implemented for events\n");
    if (list==NULL || list->size == 0){
        return NULL;
}
    event_node* iterator = list->head;
    while( (iterator != NULL) && (strcmp(iterator->origin,origin))){
        iterator = iterator->next;
    }//while
    return iterator;
}

event_node* event_list_get(event_list* list, int index){
    if (list==NULL || list->size == 0|| list->size <= index){
        return NULL;
    }//if
    event_node* iterator = list->head;
    for(int i=0; i<(index); i++){
        iterator = iterator->next;
    }
    return iterator;
}//event_list_get
