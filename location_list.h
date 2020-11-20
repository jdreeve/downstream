/* Implements dually linked location_list of strings.
 *
 * 
 */
#ifndef LOCATION_LIST_H
#define LOCATION_LIST_H
#include <stdbool.h>

typedef struct LOCATION_NODE {
    char* name;
    int x;
    int y;
    struct LOCATION_NODE *next;
    struct LOCATION_NODE *previous;
} location_node;

/* Queue structure */
typedef struct {
    location_node *head;  /* Linked location_list of elements */
    location_node *tail;
    int size;
} location_list;

/************** Operations on location_list ************************/

/*
  Create empty location_list.
  Return NULL if could not allocate space.
*/
location_list *location_list_create();

/*
  Free all storage used by location_list.
  No effect if list is NULL
*/
void location_list_destroy(location_list *list);

/*
 * Create a location_node and append it to the location_list
 * Return true if successful.
 * Return false if location_list is NULL or could not allocate space.
 */
bool location_list_add(location_list *list, char* string, int x, int y);

/*
 * Deletes a location_node from the linked list.
 * Return true if successful.
 * Return false if list is NULL or empty.
*/
bool location_list_delete(location_list *list, location_node* location_node);

/*
  Return number of elements in location_list.
  Return 0 if list is NULL or empty
 */
int location_list_size(location_list *list);

double location_list_get_distance(location_node* origin, location_node* destination);

void location_list_print(location_list *list);

/*
 * Return pointer to location_node containing string.
 * Returns NULL if list is NULL or empty.
 */
location_node* location_list_search(location_list *list, char* string);

/*
 * Return pointer to location_node at given index.
 * Returns NULL if list is NULL or empty.
*/
location_node* location_list_get(location_list *list, int index);
#endif
