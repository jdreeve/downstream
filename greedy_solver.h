#ifndef GREEDY_SOLVER_H
#define GREEDY_SOLVER_H

#include "location_list.h"
#include "event_list.h"
#include "vehicle_list.h"
#include "planned_event_list.h"

void run_greedy_solver(location_list* locations, event_list* events, vehicle_list* vehicles);

void generate_events_departure_time(event_list* events);

double get_travel_time(location_node* origin, location_node* destination);

int calculate_planned_departure_time(vehicle_list* vehicles, event_node* event, int vehicle_number);

int assign_vehicle(event_node* event, vehicle_list* vehicles);

#endif
