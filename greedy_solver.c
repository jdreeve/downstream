#include <stdio.h>
#include <math.h>
#include "greedy_solver.h"
#include <limits.h>

#define EARLY_ARRIVAL_MAX 20
#define WAIT_MAX 15
#define VELOCITY 1.0
void run_greedy_solver(location_list* locations, event_list* events, vehicle_list* vehicles){
	printf("Greedy solver running\n");
	fflush(stdout);
	generate_events_departure_time(events);

	planned_event_list* planned_events = planned_event_list_create();

	printf("Departures generated\n");
	fflush(stdout);
	events = event_list_sort_by_departure_time(events, locations);
	printf("Event list sorted\n");
	fflush(stdout);

	event_node* tailfirst_iterator = events->tail;
	while(tailfirst_iterator != NULL){
		int vehicle_number = assign_vehicle(tailfirst_iterator, vehicles);
		int planned_departure_time = calculate_planned_departure_time(vehicles, tailfirst_iterator, vehicle_number);
		int planned_arrival_time = planned_departure_time + ceil(get_travel_time(tailfirst_iterator->origin, tailfirst_iterator->destination));
		vehicle_node* current_vehicle = vehicle_list_search(vehicles, vehicle_number);
		current_vehicle->current_time = planned_arrival_time;

		planned_event_list_add(planned_events, locations, tailfirst_iterator->client_name, vehicle_number, tailfirst_iterator->origin->name, tailfirst_iterator->destination->name, planned_departure_time, planned_arrival_time);

		tailfirst_iterator = tailfirst_iterator->previous;

	printf("Planned events list created\n");
	fflush(stdout);

	planned_event_list_print(planned_events);
	}
}

int assign_vehicle(event_node* event, vehicle_list* vehicles){
	vehicle_node* current_vehicle = vehicles->head;
	int least_expensive_route_cost = INT_MAX;
	int least_expensive_vehicle_number = -1;
	int current_route_cost;

	while(NULL != current_vehicle){
		if(current_vehicle->current_time > event->departure_time){
			continue;
		}
		current_route_cost = 0;
		int current_route_distance = location_list_get_distance(event->origin, event->destination);
		int vehicle_distance_from_origin = location_list_get_distance(current_vehicle->current_location, event->origin);
		if(vehicle_distance_from_origin > (event->departure_time - current_vehicle->current_time)){
			current_route_cost += vehicle_distance_from_origin - (event->departure_time - current_vehicle->current_time);
		}
		current_route_cost += current_route_distance;

		if(current_route_cost < least_expensive_route_cost){
			least_expensive_route_cost = current_route_cost;
			least_expensive_vehicle_number = current_vehicle->vehicle_number;
		}
		current_vehicle = current_vehicle->next;
	}

	return least_expensive_vehicle_number;
}

int calculate_planned_departure_time(vehicle_list* vehicles, event_node* event, int vehicle_number){
	vehicle_node* current_vehicle = vehicle_list_search(vehicles, vehicle_number);
	int vehicle_distance_from_origin = location_list_get_distance(current_vehicle->current_location, event->origin);
	return vehicle_distance_from_origin + current_vehicle->current_time;

}

void generate_events_departure_time(event_list* events){
	int travel_time_ceiling;

	event_node* current_event = events->head;

	while(current_event != NULL){
		if(-1 == current_event->departure_time){
			travel_time_ceiling = ceil(get_travel_time(current_event->origin, current_event->destination));
			current_event->departure_time = current_event->arrival_time - travel_time_ceiling;
		}
		current_event = current_event->next;
	}
}

double get_travel_time(location_node* origin, location_node* destination){
	double travel_time;
	double distance;
	double velocity = VELOCITY;

	distance = location_list_get_distance(origin, destination);

	travel_time = distance / velocity;
	travel_time += EARLY_ARRIVAL_MAX;
	return travel_time;
}
