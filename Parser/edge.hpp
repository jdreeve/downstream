#ifndef EDGE_H
#define EDGE_H

#include "node.hpp"

using namespace std;

class Edge{
    public:
        Edge(){
        }

        Edge(Node origin, Node destination, double departureTime){
            this->origin = origin;
            this->destination = destination;
            this->departureTime = departureTime;
        }

        Edge(Node origin, Node destination, double departureTime, double arrivalTime){
            this->origin = origin;
            this->destination = destination;
            this->departureTime = departureTime;
            this->arrivalTime = arrivalTime;
        }
 
        Node getOrigin();
        Node getDestination();
        double getDepartureTime();
        double getArrivalTime();

        void setOrigin(Node origin);
        void setDestination(Node destination);
        void setDepartureTime(double departureTime);
        void setArrivalTime(double arrivalTime);

    private:
        Node origin;
        Node destination;
        double departureTime;
        double arrivalTime;
};

#endif
