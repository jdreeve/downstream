#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "location_list.h"

location_list* location_list_create(){
    location_list* list = (location_list*)malloc(sizeof(location_list));
    if (list != NULL){
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    }//if
    return list;
}//location_list_create

void location_list_destroy(location_list* list){
    /* How about freeing the list elements? */
    if (list != NULL && list->head != NULL){
            location_node* current_location_node = list->head;
            while(current_location_node->next != NULL){
                location_node* next_location_node = current_location_node->next;
		free(current_location_node->name);
                free(current_location_node);
                current_location_node=next_location_node;
        }//while
        free(current_location_node);
    }//if
    /* Free listueue structure */
    if(list!= NULL){
    free(list);
    }//if
}//list_free

bool location_list_add(location_list* list, char* name, int x, int y){
    location_node *new_location_node;
        if(list==NULL){
    return false;
        }
    new_location_node = (location_node*)malloc(sizeof(location_node));
        if(new_location_node==NULL){
    return false;
        }
    new_location_node->name = (char*)malloc((strlen(name)+1) * sizeof(char));
    strcpy(new_location_node->name, name);
    new_location_node->x = x;
    new_location_node->y = y;
    new_location_node->next = list->head;
    list->head = new_location_node;
    if (list->size==0){
        list->tail = new_location_node;
    }//if
    else{
        new_location_node->next->previous = new_location_node;
    }
    list->size++;
        return true;
}

bool location_list_delete(location_list* list, location_node* location_node){
    if (list == NULL || location_node == NULL){
        return false;
    }//if
    if (list->head==location_node){//if location_node is head
        list->head = list->head->next;
	if (location_node->next != NULL){//if there are others
	    location_node->next->previous = NULL;
	}//if
    }//if
    else if (list->tail==location_node){//if location_node is tail
        list->tail = list->tail->previous;
	if (location_node->previous != NULL){//if there are others
	    location_node->previous->next = NULL;
	}//if
    }//if
    else {//if location_node is mid-list
        location_node->previous->next = location_node->next;
	location_node->next->previous = location_node->previous;
    }//else
    free(location_node->name);
    free(location_node);
    list->size--;
    return true;
}//location_list_delete

int location_list_size(location_list* list){
    if (list==NULL){
        return 0;
    }
    return list->size;
}//location_list_size

void location_list_print(location_list* list){
	location_node* iterator = list->head;
	while(iterator != NULL){
		printf("%s x: %d y: %d\n", iterator->name, iterator->x, iterator->y);
		iterator = iterator->next;
	}
}

location_node* location_list_search(location_list* list, char* string){
    if (list==NULL || list->size == 0){
        return NULL;
}
    location_node* iterator = list->head;
    while( (iterator != NULL) && (strcmp(iterator->name,string))){
        iterator = iterator->next;
    }//while
    return iterator;
}

double location_list_get_distance(location_node* origin, location_node* destination){
	int delta_x = (destination->x) - (origin->x);
	int delta_y = (destination->y) - (origin->y);
	double distance = sqrt((double)(delta_x * delta_x) + (double)(delta_y * delta_y));
	return distance;
}

location_node* location_list_get(location_list* list, int index){
    if (list==NULL || list->size == 0|| list->size <= index){
        return NULL;
    }//if
    location_node* iterator = list->head;
    for(int i=0; i<(index); i++){
        iterator = iterator->next;
    }
    return iterator;
}//location_list_get
