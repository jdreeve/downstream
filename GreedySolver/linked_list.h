/* Implements dually linked linked_list of strings.
 *
 * 
 */
#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include <stdbool.h>

typedef struct ELE {
    char* data;
    struct ELE *next;
    struct ELE *previous;
} node;

/* Queue structure */
typedef struct {
    node *head;  /* Linked linked_list of elements */
    node *tail;
    int size;
} linked_list;

/************** Operations on linked_list ************************/

/*
  Create empty linked_list.
  Return NULL if could not allocate space.
*/
linked_list *linked_list_create();

/*
  Free all storage used by linked_list.
  No effect if list is NULL
*/
void linked_list_destroy(linked_list *list);

/*
 * Create a node and append it to the linked_list
 * Return true if successful.
 * Return false if linked_list is NULL or could not allocate space.
 */
bool linked_list_add(linked_list *list, char* string);

/*
 * Deletes a node from the linked list.
 * Return true if successful.
 * Return false if list is NULL or empty.
*/
bool linked_list_delete(linked_list *list, node* node);

/*
  Return number of elements in linked_list.
  Return 0 if list is NULL or empty
 */
int linked_list_size(linked_list *list);

/*
 * Return pointer to node containing string.
 * Returns NULL if list is NULL or empty.
 */
node* linked_list_search(linked_list *list, char* string);

/*
 * Return pointer to node at given index.
 * Returns NULL if list is NULL or empty.
*/
node* linked_list_get(linked_list *list, int index);
#endif
