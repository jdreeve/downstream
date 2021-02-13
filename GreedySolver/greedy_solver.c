#include <stdio.h>
#include <math.h>
#include "greedy_solver.h"
#include <limits.h>

#define DEBUG 1

#define EARLY_ARRIVAL_MAX 20
#define WAIT_MAX 15
#define VELOCITY 1.0
void run_greedy_solver(location_list* locations, event_list* events, vehicle_list* vehicles){
	if(DEBUG){
        printf("Greedy solver running\n");
	    fflush(stdout);
    }
	generate_events_departure_time(events);

	planned_event_list* planned_events = planned_event_list_create();
    if(DEBUG){
	    printf("Departures generated\n");
	    fflush(stdout);
    }
	events = event_list_sort_by_departure_time(events, locations);
    if(DEBUG){
    printf("Event list sorted\n");
	fflush(stdout);
    }

	event_node* events_iterator = events->tail;
	while(events_iterator != NULL){
        if(DEBUG){
            printf("Processing next event.\n");
        }
		int vehicle_number = assign_vehicle(events_iterator, vehicles);
        if(DEBUG){
            printf("Vehicle number assigned: %d\n", vehicle_number);
        }
		double planned_departure_time = calculate_planned_departure_time(vehicles, events_iterator, vehicle_number);
        if(DEBUG){
            printf("Planned departure time assigned: %f\n", planned_departure_time);
        }
		double planned_arrival_time = planned_departure_time + ceil(get_travel_time(events_iterator->origin, events_iterator->destination));
        if(DEBUG){
            printf("Planned arrival time assigned: %d\n", planned_arrival_time);
        }
		vehicle_node* current_vehicle = vehicle_list_search(vehicles, vehicle_number);
		current_vehicle->current_time = (double)planned_arrival_time;
        current_vehicle->current_location = events_iterator->destination;
        if(DEBUG){
            if(current_vehicle != NULL){
                printf("Vehicle assigned: %d\tVehicle time is now: %f\t\n", current_vehicle->vehicle_number, current_vehicle->current_time);
            }
            else
                printf("Vehicle %d not found!\n", vehicle_number);
        } 
        if(DEBUG){
            printf("Vehicle %d now arriving at %s at time %f\n", current_vehicle->vehicle_number, current_vehicle->current_location->name, current_vehicle->current_time);
        }
		planned_event_list_add(planned_events, locations, events_iterator->client_name, vehicle_number, events_iterator->origin->name, events_iterator->destination->name, planned_departure_time, planned_arrival_time);

		events_iterator = events_iterator->previous;
        if(DEBUG){
    	    printf("Planned events list created\n");
	        planned_event_list_print(planned_events);
	        fflush(stdout);
        }
	}
}

int assign_vehicle(event_node* event, vehicle_list* vehicles){
	vehicle_node* current_vehicle = vehicles->head;
	double least_expensive_route_cost = INT_MAX;
	int least_expensive_vehicle_number = -1;
	double current_route_cost;

	while(NULL != current_vehicle){
		if(current_vehicle->current_time > event->departure_time){
            printf("Continuing\n");
			continue;
		}
		current_route_cost = 0;
        if(DEBUG){
            printf("Calculating cost for vehicle %d\n", current_vehicle->vehicle_number);
        }
        if(DEBUG){
            printf("Calculating current route distance.\n");
        }
		double current_route_distance = location_list_get_distance(event->origin, event->destination);
        if(DEBUG){
            printf("Route distance = %f\n", current_route_distance);
        }
		double vehicle_distance_from_origin = location_list_get_distance(current_vehicle->current_location, event->origin);
        if(DEBUG){
            printf("Vehicle %d location: %s\nDistance from route origin = %f\n", current_vehicle->vehicle_number, current_vehicle->current_location->name, vehicle_distance_from_origin);
        }
		//if(vehicle_distance_from_origin > (event->departure_time - current_vehicle->current_time)){
			current_route_cost += vehicle_distance_from_origin - (event->departure_time - current_vehicle->current_time);
		//}
		current_route_cost += current_route_distance;
        if(DEBUG){
            printf("Current route cost: %f\n", current_route_cost);
            printf("Least expensive route cost: %f\n", least_expensive_route_cost);
        }

		if(current_route_cost < least_expensive_route_cost){
			least_expensive_route_cost = current_route_cost;
			least_expensive_vehicle_number = current_vehicle->vehicle_number;
		}
		current_vehicle = current_vehicle->next;
	}

    if(DEBUG){
        printf("Vehicle assigned: %d\n", least_expensive_vehicle_number);
    }
	return least_expensive_vehicle_number;
}

int calculate_planned_departure_time(vehicle_list* vehicles, event_node* event, int vehicle_number){
	vehicle_node* current_vehicle = vehicle_list_search(vehicles, vehicle_number);
	int vehicle_distance_from_origin = location_list_get_distance(current_vehicle->current_location, event->origin);
    int planned_departure_time = vehicle_distance_from_origin + current_vehicle->current_time;
    if(DEBUG){
        printf("calculate_planned_departure_time reports:\nPlanned departure time: %d\n---\n", planned_departure_time);
    }
    return planned_departure_time;
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
