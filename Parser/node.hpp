#ifndef NODE_H
#define NODE_H
#include <string>

using namespace std;

class Node{
    public:
        string name;
        int earliestServiceTime;
        int latestServiceTime;
        int serviceDuration;
        string address;
        int load;
        int nodeID;

        Node(){
            name = "";
            earliestServiceTime = 0;
            latestServiceTime = 1440;
            serviceDuration = 0;
            address = "";
            load = 0;
        }
        Node(const string n, int early, int late, int ld, string addr, int nodeID){
            this->name = n;
            this->earliestServiceTime = early;
            this->latestServiceTime = late;
            this->load = ld;
            this->address = addr;
            this->serviceDuration = 0;
            this->nodeID = nodeID;
        }
};
#endif
