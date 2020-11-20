#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
            node* current_node = list->head;
            while(current_node->next != NULL){
                node* next_node = current_node->next;
		free(current_node->name);
                free(current_node);
                current_node=next_node;
        }//while
        free(current_node);
    }//if
    /* Free listueue structure */
    if(list!= NULL){
    free(list);
    }//if
}//list_free

bool location_list_add(location_list* list, char* name, int x, int y){
    node *new_node;
        if(list==NULL){
    return false;
        }
    new_node = (node*)malloc(sizeof(node));
        if(new_node==NULL){
    return false;
        }
    new_node->name = (char*)malloc((strlen(name)+1) * sizeof(char));
    strcpy(new_node->name, name);
    new_node->x = x;
    new_node->y = y;
    new_node->next = list->head;
    list->head = new_node;
    if (list->size==0){
        list->tail = new_node;
    }//if
    else{
        new_node->next->previous = new_node;
    }
    list->size++;
        return true;
}

bool location_list_delete(location_list* list, node* node){
    if (list == NULL || node == NULL){
        return false;
    }//if
    if (list->head==node){//if node is head
        list->head = list->head->next;
	if (node->next != NULL){//if there are others
	    node->next->previous = NULL;
	}//if
    }//if
    else if (list->tail==node){//if node is tail
        list->tail = list->tail->previous;
	if (node->previous != NULL){//if there are others
	    node->previous->next = NULL;
	}//if
    }//if
    else {//if node is mid-list
        node->previous->next = node->next;
	node->next->previous = node->previous;
    }//else
    free(node->name);
    free(node);
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
	node* iterator = list->head;
	while(iterator != NULL){
		printf("%s x: %d y: %d\n", iterator->name, iterator->x, iterator->y);
		iterator = iterator->next;
	}
}

node* location_list_search(location_list* list, char* string){
    if (list==NULL || list->size == 0){
        return NULL;
}
    node* iterator = list->head;
    while( (iterator != NULL) && (strcmp(iterator->name,string))){
        iterator = iterator->next;
    }//while
    return iterator;
}

node* location_list_get(location_list* list, int index){
    if (list==NULL || list->size == 0|| list->size <= index){
        return NULL;
    }//if
    node* iterator = list->head;
    for(int i=0; i<(index); i++){
        iterator = iterator->next;
    }
    return iterator;
}//location_list_get
