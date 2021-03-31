#ifndef EDGE_H
#define EDGE_H

#include <iostream>
#include "node.hpp"

using namespace std;

class Edge{
    public:
        Edge(){
        }

        Edge(Node origin, Node destination){
            this->setOrigin(origin);
            this->setDestination(destination);
        }

        Edge(Node origin, Node destination, double departureTime, double arrivalTime){
            this->origin = origin;
            this->destination = destination;
            this->departureTime = departureTime;
            this->arrivalTime = arrivalTime;
        }
 
        Node getOrigin(){
            return this->origin;
        }
        Node getDestination(){
            return this->destination;
        }
        double getDepartureTime(){
            return this->departureTime;
        }

        double getArrivalTime(){
            return this->arrivalTime;
        }

        void setOrigin(Node origin){
            this->origin = origin;
        }
        void setDestination(Node destination){
            this->destination = destination;
        }
        void setDepartureTime(double departureTime){
            this->departureTime = departureTime;
        }
        void setArrivalTime(double arrivalTime){
            this->arrivalTime = arrivalTime;
        }
        void printEdge(){
            cout << "Printing edge:" << "\n";
            cout << "\tOrigin node address: " << this->origin.address << "\n" << "\tOrigin node ID: " << this->origin.nodeID << "\n";
            cout << "\tDestination node address: " << this->destination.address << "\n" << "\tDestination node ID: " << this->destination.nodeID << "\n";
            cout << "\tDeparture Time: " << this->departureTime << "\n";
        }

    private:
        Node origin;
        Node destination;
        double departureTime;
        double arrivalTime;
};

#endif
