/* Implements dually linked event_list of strings.
 *
 * 
 */
#ifndef EVENT_LIST_H
#define EVENT_LIST_H
#include <stdbool.h>

typedef struct EVENT_NODE {
    char* client_name;
    char* origin;
    char* destination;
    int departure_time;
    int arrival_time;
    struct EVENT_NODE *next;
    struct EVENT_NODE *previous;
} event_node;

/* Queue structure */
typedef struct {
    event_node *head;  /* Linked event_list of elements */
    event_node *tail;
    int size;
} event_list;

/************** Operations on event_list ************************/

/*
  Create empty event_list.
  Return NULL if could not allocate space.
*/
event_list *event_list_create();

/*
  Free all storage used by event_list.
  No effect if list is NULL
*/
void event_list_destroy(event_list *list);

/*
 * Create a event_node and append it to the event_list
 * Return true if successful.
 * Return false if event_list is NULL or could not allocate space.
 */
bool event_list_add(event_list *list, char* client_name, char* origin, char* destination, int departure_time, int arrival_time);

/*
 * Deletes a event_node from the linked list.
 * Return true if successful.
 * Return false if list is NULL or empty.
*/
bool event_node_delete(event_list *list, event_node* event_node);

/*
  Return number of elements in event_list.
  Return 0 if list is NULL or empty
 */
int event_list_size(event_list *list);

void event_list_print(event_list* list);

/* TODO not supported for event list!
 * Return pointer to event_node containing string.
 * Returns NULL if list is NULL or empty.
 */
event_node* event_list_search(event_list *list, char* string);

/*
 * Return pointer to event_node at given index.
 * Returns NULL if list is NULL or empty.
*/
event_node* event_list_get(event_list *list, int index);
#endif
