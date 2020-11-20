#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vehicle_list.h"

vehicle_list* vehicle_list_create(location_node* home){
    vehicle_list* list = (vehicle_list*)malloc(sizeof(vehicle_list));
    if (list != NULL){
    list->head = NULL;
    list->tail = NULL;
    list->home = home;
    list->size = 0;
    }//if
    return list;
}//vehicle_list_create

void vehicle_list_set_home(vehicle_list* list, location_node* home){
	vehicle_node* iterator = list->head;
	while(iterator!=NULL){
		iterator->current_location = home;
		iterator = iterator->next;
	}
}

void vehicle_list_destroy(vehicle_list* list){
    /* How about freeing the list elements? */
    if (list != NULL && list->head != NULL){
            vehicle_node* current_vehicle_node = list->head;
            while(current_vehicle_node->next != NULL){
                vehicle_node* next_vehicle_node = current_vehicle_node->next;
                free(current_vehicle_node);
                current_vehicle_node=next_vehicle_node;
        }//while
        free(current_vehicle_node);
    }//if
    /* Free listueue structure */
    if(list!= NULL){
    free(list);
    }//if
}//list_free

void vehicle_list_print(vehicle_list* list){
	vehicle_node* iterator;
	iterator = list->head;
	while(iterator!= NULL){
	printf("Vehicle %d capacity %d\n", iterator->vehicle_number, iterator->capacity);
	if(iterator->current_location != NULL){
		printf("location: %s\n", iterator->current_location->name);
	}
	iterator = iterator->next;
	}
}

bool vehicle_list_add(vehicle_list* list, int capacity){
    vehicle_node *new_vehicle_node;
        if(list==NULL){
    return false;
        }
    new_vehicle_node = (vehicle_node*)malloc(sizeof(vehicle_node));
        if(new_vehicle_node==NULL){
    return false;
        }
    new_vehicle_node->vehicle_number = list->size;
    new_vehicle_node->capacity = capacity;
    new_vehicle_node->current_time = 0;
    new_vehicle_node->current_location = list->home;
    new_vehicle_node->next = list->head;
    list->head = new_vehicle_node;
    if (list->size==0){
        list->tail = new_vehicle_node;
    }//if
    else{
        new_vehicle_node->next->previous = new_vehicle_node;
    }
    list->size++;
        return true;
}

bool vehicle_list_delete(vehicle_list* list, vehicle_node* vehicle_node){
    if (list == NULL || vehicle_node == NULL){
        return false;
    }//if
    if (list->head==vehicle_node){//if vehicle_node is head
        list->head = list->head->next;
	if (vehicle_node->next != NULL){//if there are others
	    vehicle_node->next->previous = NULL;
	}//if
    }//if
    else if (list->tail==vehicle_node){//if vehicle_node is tail
        list->tail = list->tail->previous;
	if (vehicle_node->previous != NULL){//if there are others
	    vehicle_node->previous->next = NULL;
	}//if
    }//if
    else {//if vehicle_node is mid-list
        vehicle_node->previous->next = vehicle_node->next;
	vehicle_node->next->previous = vehicle_node->previous;
    }//else
    free(vehicle_node);
    list->size--;
    return true;
}//vehicle_list_delete

int vehicle_list_size(vehicle_list* list){
    if (list==NULL){
        return 0;
    }
    return list->size;
}//vehicle_list_size

vehicle_node* vehicle_list_search(vehicle_list* list, int target_number){
    if (list==NULL || list->size == 0){
        return NULL;
}
    vehicle_node* iterator = list->head;
    while( (iterator != NULL) && (iterator->vehicle_number != target_number)){
        iterator = iterator->next;
    }//while
    return iterator;
}

vehicle_node* vehicle_list_get(vehicle_list* list, int index){
    if (list==NULL || list->size == 0|| list->size <= index){
        return NULL;
    }//if
    vehicle_node* iterator = list->head;
    for(int i=0; i<(index); i++){
        iterator = iterator->next;
    }
    return iterator;
}//vehicle_list_get
