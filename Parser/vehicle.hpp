#ifndef VEHICLE_H
#define VEHICLE_H
#include <string>

using namespace std;

class Vehicle{
    public:
        string name;
        int capacity;
        int maxTourTime;
        int ID;

        Vehicle(string n, int cap, int maxTime){
            name = n;
            capacity = cap;
            maxTourTime = maxTime;
        }
};
#endif
