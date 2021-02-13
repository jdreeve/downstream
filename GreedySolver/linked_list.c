#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_list.h"

linked_list* linked_list_create(){
    linked_list* list = (linked_list*)malloc(sizeof(linked_list));
    if (list != NULL){
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    }//if
    return list;
}//linked_list_create

void linked_list_destroy(linked_list* list){
    /* How about freeing the list elements? */
    if (list != NULL && list->head != NULL){
            node* current_node = list->head;
            while(current_node->next != NULL){
                node* next_node = current_node->next;
		free(current_node->data);
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

bool linked_list_add(linked_list* list, char* string){
    node *newh;
        if(list==NULL){
    return false;
        }
    newh = (node*)malloc(sizeof(node));
        if(newh==NULL){
    return false;
        }
    newh->data = (char*)malloc((strlen(string)+1) * sizeof(char));
    strcpy(newh->data, string);
    newh->next = list->head;
    list->head = newh;
    if (list->size==0){
        list->tail = newh;
    }//if
    else{
        newh->next->previous = newh;
    }
    list->size++;
        return true;
}

bool linked_list_delete(linked_list* list, node* node){
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
    free(node->data);
    free(node);
    list->size--;
    return true;
}//linked_list_delete

int linked_list_size(linked_list* list){
    if (list==NULL){
        return 0;
    }
    return list->size;
}//linked_list_size

node* linked_list_search(linked_list* list, char* string){
    if (list==NULL || list->size == 0){
        return NULL;
}
    node* iterator = list->head;
    while( (iterator != NULL) && (strcmp(iterator->data,string))){
        iterator = iterator->next;
    }//while
    return iterator;
}

node* linked_list_get(linked_list* list, int index){
    if (list==NULL || list->size == 0|| list->size <= index){
        return NULL;
    }//if
    node* iterator = list->head;
    for(int i=0; i<(index); i++){
        iterator = iterator->next;
    }
    return iterator;
}//linked_list_get
