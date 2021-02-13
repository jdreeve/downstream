#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#define REQUIREMENTS_FILE "./requirements.csv"
#define VEHICLES_FILE "./vehicles.csv"
#define LP_FILE "requirements.lp"
#define DELIMITER ","

#define VEHICLE_COUNT 10
#define VEHICLE_CAPACITY 8
#define VEHICLE_MAX_TIME 20000

#define EARLY_WINDOW 10
#define LATE_WINDOW 0

using namespace std;

class Node{
    public:
        string name;
        int earliestServiceTime;
        int latestServiceTime;
        string address;
        int load;

        Node(const string n, int early, int late, int ld, string addr){
            name = n;
            earliestServiceTime = early;
            latestServiceTime = late;
            load = ld;
            address = addr;
        }
};

class Vehicle{
    public:
        string name;
        int capacity;
        int maxTourTime;

        Vehicle(string n, int cap, int maxTime){
            name = n;
            capacity = cap;
            maxTourTime = maxTime;
        }
};

void parseRequirements(vector<Node> &nodes);
void getVehicles(vector<Vehicle> &vehicles);
void writeLPFile(std::string filePath, vector<Node> nodes, vector<Vehicle> vehicles);
string getObjective(vector<Node> nodes, vector<Vehicle> vehicles);
void writeObjective(std::string filePath, vector<Node> nodes, vector<Vehicle> vehicles);
void writeConstraints(std::string filePath, vector<Node> nodes, vector<Vehicle> vehicles);
void writeBinarySection(std::string filePath, vector<Node> nodes, vector<Vehicle> vehicles);
void writeTerminal(std::string filePath);
void printNodes(vector<Node> nodes);
void printVehicles(vector<Vehicle> vehicles);
string cordeauConstraint2(vector<Node> nodes, vector<Vehicle> vehicles);

int main() {
    vector<Node> nodes;
    vector<Vehicle> vehicles;
    parseRequirements(nodes);
    getVehicles(vehicles);
    string objective = getObjective(nodes, vehicles);
    writeLPFile(LP_FILE, nodes, vehicles);
}

void parseRequirements(vector<Node> &nodes){
    vector<Node> origins;
    vector<Node> destinations;
    string line;
    ifstream requirements(REQUIREMENTS_FILE);
    while(getline(requirements, line)){//parse each line
        string delimiter = DELIMITER;
        string name;
        string address;
        string comment;
        int dropoffTime;
        int pickupTime;
        int earliestServiceTime;
        int latestServiceTime;
        double load;
        size_t tokenStart = 0;
        size_t tokenEnd = 0;
        
        //get name
        tokenEnd = line.find(delimiter, tokenStart);
        name = line.substr(tokenStart, tokenEnd - tokenStart);
        
        //get load
        tokenStart = tokenEnd + delimiter.length();
        tokenEnd = line.find(delimiter, tokenStart);
        load = stoi(line.substr(tokenStart, tokenEnd - tokenStart));

        //get dropoff time
        tokenStart = tokenEnd + delimiter.length();
        tokenEnd = line.find(delimiter, tokenStart);
        dropoffTime = stoi(line.substr(tokenStart, tokenEnd - tokenStart));

        //get pickup time
        tokenStart = tokenEnd + delimiter.length();
        tokenEnd = line.find(delimiter, tokenStart);
        pickupTime = stoi(line.substr(tokenStart, tokenEnd - tokenStart));

        //get address
        tokenStart = tokenEnd + delimiter.length();
        tokenEnd = line.find(delimiter, tokenStart);
        address = line.substr(tokenStart, line.size());

        
        //TODO build clients vector to track client locations,
            //implement traveling from non-FIRST to non-FIRST location

        //string origin = getOrigin(name, pickupTime, clients);
        string origin="<DEBUG>FIRST at Blue Ridge";
        //push origin node
        //TODO ensure earliest/latest service times consistent with 24 hour time (no negatives, no > 2359)
        earliestServiceTime = dropoffTime - EARLY_WINDOW; 
        latestServiceTime = dropoffTime + LATE_WINDOW;

        Node temp = Node(name, earliestServiceTime, latestServiceTime, load, origin);
        origins.push_back(temp);

        //push destination node
        earliestServiceTime = pickupTime - EARLY_WINDOW; 
        latestServiceTime = pickupTime + LATE_WINDOW;

        temp = Node(name, earliestServiceTime, latestServiceTime, load, address);
        destinations.push_back(temp);
    }
    nodes.insert(nodes.end(), origins.begin(), origins.end());
    nodes.insert(nodes.end(), destinations.begin(), destinations.end());

    requirements.close();
    //vector<Client> client //stores current locations of clients
    //go through file to create origins and destinations
    //
    //nodes.push_back(FIRST) //add origin node
    //nodes.insert(nodes.end(), origins.begin(), origins.end());
    //nodes.insert(nodes.end(), destinations.begin(), destinations.end());
    //nodes.push_back(FIRST) //add destination node
}

void printNodes(vector<Node> nodes){
    cout << "<<<<Printing nodes>>>>\n";
    for(unsigned i=0; i < nodes.size(); i++){
        cout << "Name: " << nodes[i].name << "\n";
        cout << "Early: " << nodes[i].earliestServiceTime << "\n";
        cout << "Late: " << nodes[i].latestServiceTime << "\n";
        cout << "Load: " << nodes[i].load << "\n";
        cout << "Address: " << nodes[i].address << "\n";
    }
}

void getVehicles(vector<Vehicle> &vehicles){
    ifstream vehicleFile(VEHICLES_FILE);
    string line;
    while(getline(vehicleFile, line)){//parse each line
        size_t tokenStart = 0;
        size_t tokenEnd = 0;
        string vehicleName;
        int capacity;
        string delimiter=DELIMITER;
        //get vehicle name
        tokenEnd = line.find(delimiter, tokenStart);
        vehicleName = line.substr(tokenStart, tokenEnd - tokenStart);
        tokenStart = tokenEnd + delimiter.length();
        string capstring = line.substr(tokenStart, line.size());
        capacity = stoi(line.substr(tokenStart, line.size()));

        Vehicle temp(vehicleName, capacity, VEHICLE_MAX_TIME);
        vehicles.push_back(temp);
    }
}

void printVehicles(vector<Vehicle> vehicles){
    cout << "<<<<Printing vehicles>>>>\n";
    for(unsigned i=0; i < vehicles.size(); i++){
        cout << "Vehicle name: " << vehicles[i].name << "\n";
        cout << "Capacity: " << vehicles[i].capacity << "\n";
        cout << "Max tour time: " << vehicles[i].maxTourTime<< "\n";
    }
}

void writeLPFile(std::string filePath, vector<Node> nodes, vector<Vehicle> vehicles){
    writeObjective(filePath, nodes, vehicles);
    writeConstraints(filePath, nodes, vehicles);
    writeBinarySection(filePath, nodes, vehicles);
    writeTerminal(filePath);
}

string getObjective(vector<Node> nodes, vector<Vehicle> vehicles){
    string objective;
    int charCounter = 0;
    for(unsigned i=0; i< nodes.size(); i++){
        for(unsigned j=0; j < nodes.size(); j++){
            if(i!=j){
                for(unsigned k=0; k<vehicles.size(); k++){
                    string tempString;
                    //TODO add distance coefficient
                    tempString += "x";
                    tempString += to_string(i);
                    tempString += to_string(j);
                    tempString += to_string(k);
                    tempString += " + ";
                    objective.append(tempString);
                    charCounter += tempString.length();
                    if(charCounter>200){
                        objective.append("\n");
                        charCounter = 0;
                    }
                }
            }
        }
    }
    objective.erase(objective.length()-3);
    return objective;
}

string getBinaryVars(vector<Node> nodes, vector<Vehicle> vehicles){
    string binaries;

    int charCounter = 0;
    for(unsigned i=0; i< nodes.size(); i++){
        for(unsigned j=0; j < nodes.size(); j++){
            if(i!=j){
                for(unsigned k=0; k<vehicles.size(); k++){
                    string tempString;
                    //TODO add distance coefficient
                    tempString += "x";
                    tempString += to_string(i);
                    tempString += to_string(j);
                    tempString += to_string(k);
                    tempString += " " ;
                    binaries.append(tempString);
                    charCounter += tempString.length();
                    if(charCounter>200){
                        binaries.append("\n");
                        charCounter = 0;
                    }
                }
            }
        }
    }
    

    return binaries;
}

void writeObjective(std::string filePath, vector<Node> nodes, vector<Vehicle> vehicles){
    string objective;
    objective = getObjective(nodes, vehicles);
    ofstream lpfile;
    lpfile.open(filePath);
    lpfile << "MINIMIZE\n";
    lpfile << objective;
    lpfile << "\n";
    lpfile.close();
}

void writeBinarySection(std::string filePath, vector<Node> nodes, vector<Vehicle> vehicles){
    string binaries = getBinaryVars(nodes, vehicles);
    ofstream lpfile;
    lpfile.open(filePath, ios_base::app);
    lpfile << "BINARY\n";
    lpfile << binaries;
    lpfile << "\n";
    lpfile.close();
}

void writeTerminal(std::string filePath){
    ofstream lpfile;
    lpfile.open(filePath, ios_base::app);
    lpfile << "END";
    lpfile.close();
}

void writeConstraints(std::string filePath, vector<Node> nodes, vector<Vehicle> vehicles){
    string constraint;
    ofstream lpfile;
    lpfile.open(filePath, ios_base::app);
    lpfile << "SUBJECT TO\n";
    constraint=cordeauConstraint2(nodes, vehicles);
    lpfile << constraint;
    lpfile << "\n";
    lpfile.close();
}

string cordeauConstraint2(vector<Node> nodes, vector<Vehicle> vehicles){
    string constraint;
    string operation = " = 1\n";

    for(unsigned i=1; i<(nodes.size()-2)/2; i++){
        for(unsigned k=0; k<vehicles.size(); k++){
            for(unsigned j=0; j<nodes.size(); j++){
                if(i!=j){
                    string tempString;
                    tempString += "x";
                    tempString += to_string(i);
                    tempString += to_string(j);
                    tempString += to_string(k);
                    tempString.append(operation);
                    constraint.append(tempString);
                }
            }
        }
    }
    return constraint;
}
