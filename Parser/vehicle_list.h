/* Implements dually linked vehicle_list of strings.
 *
 * 
 */
#ifndef VEHICLE_LIST_H
#define VEHICLE_LIST_H
#include <stdbool.h>
#include "location_list.h"
typedef struct VEHICLE {
    int vehicle_number;
    int capacity;
    double current_time;
    location_node* current_location;
    struct VEHICLE *next;
    struct VEHICLE *previous;
} vehicle_node;

/* Queue structure */
typedef struct {
    vehicle_node *head;  /* Linked vehicle_list of elements */
    vehicle_node *tail;
    location_node *home;
    int size;
} vehicle_list;

/************** Operations on vehicle_list ************************/

/*
  Create empty vehicle_list.
  Return NULL if could not allocate space.
*/
vehicle_list *vehicle_list_create(location_node* home);

void vehicle_list_set_home(vehicle_list* list, location_node*home);

/*
  Free all storage used by vehicle_list.
  No effect if list is NULL
*/
void vehicle_list_destroy(vehicle_list *list);

void vehicle_list_print(vehicle_list *list);

/*
 * Create a vehicle_node and append it to the vehicle_list
 * Return true if successful.
 * Return false if vehicle_list is NULL or could not allocate space.
 */
bool vehicle_list_add(vehicle_list *list, int capacity);

/*
 * Deletes a vehicle_node from the linked list.
 * Return true if successful.
 * Return false if list is NULL or empty.
*/
bool vehicle_list_delete(vehicle_list *list, vehicle_node* vehicle_node);

/*
  Return number of elements in vehicle_list.
  Return 0 if list is NULL or empty
 */
int vehicle_list_size(vehicle_list *list);

/*
 * Return pointer to vehicle_node containing string.
 * Returns NULL if list is NULL or empty.
 */
vehicle_node* vehicle_list_search(vehicle_list *list, int vehicle_number);

/*
 * Return pointer to vehicle_node at given index.
 * Returns NULL if list is NULL or empty.
*/
vehicle_node* vehicle_list_get(vehicle_list *list, int index);
#endif
