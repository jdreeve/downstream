/* Implements dually linked planned_event_list of strings.
 *
 * 
 */
#ifndef PLANNED_EVENT_LIST_H
#define PLANNED_EVENT_LIST_H

#include <stdbool.h>
#include "location_list.h"

typedef struct PLANNED_EVENT_NODE {
    char* client_name;
    int vehicle_number;
    location_node* origin;
    location_node* destination;
    int departure_time;
    int arrival_time;
    struct PLANNED_EVENT_NODE *next;
    struct PLANNED_EVENT_NODE *previous;
} planned_event_node;

/* Queue structure */
typedef struct {
    planned_event_node *head;  /* Linked planned_event_list of elements */
    planned_event_node *tail;
    int size;
} planned_event_list;

/************** Operations on planned_event_list ************************/

/*
  Create empty planned_event_list.
  Return NULL if could not allocate space.
*/
planned_event_list *planned_event_list_create();

/*
  Free all storage used by planned_event_list.
  No effect if list is NULL
*/
void planned_event_list_destroy(planned_event_list *list);

/*
 * Create a planned_event_node and append it to the planned_event_list
 * Return true if successful.
 * Return false if planned_event_list is NULL or could not allocate space.
 */
bool planned_event_list_add(planned_event_list *list, location_list* locations, char* client_name, int vehicle_number, char* origin_name, char* destination_name, int departure_time, int arrival_time);

/*
 * Deletes a planned_event_node from the linked list.
 * Return true if successful.
 * Return false if list is NULL or empty.
*/
bool planned_event_node_delete(planned_event_list *list, planned_event_node* planned_event_node);

/*
  Return number of elements in planned_event_list.
  Return 0 if list is NULL or empty
 */
int planned_event_list_size(planned_event_list *list);

planned_event_list* planned_event_list_sort_by_departure_time(planned_event_list *list, location_list* locations);

void planned_event_list_print(planned_event_list* list);

/* TODO not supported for planned_event list!
 * Return pointer to planned_event_node containing string.
 * Returns NULL if list is NULL or empty.
 */
planned_event_node* planned_event_list_search(planned_event_list *list, char* string);

/*
 * Return pointer to planned_event_node at given index.
 * Returns NULL if list is NULL or empty.
*/
planned_event_node* planned_event_list_get(planned_event_list *list, int index);
#endif
