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
        int serviceDuration;
        string address;
        int load;

        Node(const string n, int early, int late, int ld, string addr){
            name = n;
            earliestServiceTime = early;
            latestServiceTime = late;
            load = ld;
            address = addr;
            serviceDuration = 0;
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
string cordeaucordeauConstraint3(vector<Node> nodes, vector<Vehicle> vehicles);
string cordeauConstraint4(vector<Node> nodes, vector<Vehicle> vehicles);
string cordeauConstraint5(vector<Node> nodes, vector<Vehicle> vehicles);
string cordeauConstraint6(vector<Node> nodes, vector<Vehicle> vehicles);
string cordeauConstraint7(vector<Node> nodes, vector<Vehicle> vehicles);
int getTransitTime(Node i, Node j);

string buildConstantTerm(int constant, string operation);
string buildTransitTerm(int coefficient, int origin, int destination, int vehicle, string operation);
string buildStartServiceTerm(int coefficient, int nodeIndex, int vehicle, string operation);
void addTerm(string &constraint, string term);

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
    lpfile << "\\Constraint 2\n";
    lpfile << constraint;
    lpfile << "\n";
    constraint = cordeaucordeauConstraint3(nodes, vehicles);
    lpfile << "\\Constraint 3\n";
    lpfile << constraint;
    lpfile << "\n";
    constraint = cordeauConstraint4(nodes, vehicles);
    lpfile << "\\Constraint 4\n";
    lpfile << constraint;
    lpfile << "\n";
    constraint = cordeauConstraint5(nodes, vehicles);
    lpfile << "\\Constraint 5\n";
    lpfile << constraint;
    lpfile << "\n";
    constraint = cordeauConstraint6(nodes, vehicles);
    lpfile << "\\Constraint 6\n";
    lpfile << constraint;
    lpfile << "\n";
    constraint = cordeauConstraint7(nodes, vehicles);
    lpfile << "\\Constraint 7\n";
    lpfile << constraint;
    lpfile << "\n";
    lpfile.close();
}

/* 
 * Constraint 2: Visit every pickup location (i.e. serve every request exactly once)
 */
string cordeauConstraint2(vector<Node> nodes, vector<Vehicle> vehicles){
    string cordeauConstraint2;
    string operation = " + ";

    for(unsigned i=1; i<=(nodes.size()-2)/2; i++){
        for(unsigned k=0; k<vehicles.size(); k++){
            for(unsigned j=0; j<nodes.size(); j++){
                if(i!=j){
                    string term = buildTransitTerm(1, i, j, k, " + ");
                    addTerm(cordeauConstraint2, term);
                }
            }
        }
        cordeauConstraint2.erase(cordeauConstraint2.size()-operation.size());
        cordeauConstraint2 += " = 1\n";
    }
    return cordeauConstraint2;
}

/*
 * Constraint 3: The same vehicle must visit corresponding pickup and dropoff nodes.
 */
string cordeaucordeauConstraint3(vector<Node> nodes, vector<Vehicle> vehicles){
    string cordeauConstraint3;
    string operation1 = " + ";
    string operation2 = " - ";
    int charCounter = 0;
    for(unsigned i = 1; i <= (nodes.size()-2)/2; i++){
        for(unsigned k = 0; k < vehicles.size(); k++){
            for(unsigned j = 0; j < nodes.size(); j++){
                if(i != j){
                    string term = buildTransitTerm(1, i, j, k, " + ");
                    addTerm(cordeauConstraint3, term);
                    if(charCounter>200){
                        cordeauConstraint3.append("\n");
                        charCounter = 0;       
                    }
                }
            }
            cordeauConstraint3.erase(cordeauConstraint3.size()-3);
            cordeauConstraint3 += " - ";
            for(unsigned j = 0; j < nodes.size(); j++){
                if(i != j){
                    if((((nodes.size()-2)/2))+i != j){
                        string tempString;
                        string term = buildTransitTerm(1, (nodes.size()-2)/2 +i, j, k, " - ");
                        addTerm(cordeauConstraint3, term);
                    }
                }
            }
        cordeauConstraint3.erase(cordeauConstraint3.size()-3);
        cordeauConstraint3 += " = 0\n";
        charCounter = 0;
        }
    }
    return cordeauConstraint3;
}

/*
 * Constraint 4: routes begin at origin depot (node 0).
 */
string cordeauConstraint4(vector<Node> nodes, vector<Vehicle> vehicles){
    string cordeauConstraint4;
    string operation = " + ";
    for(unsigned k=0; k<vehicles.size(); k++){
        for(unsigned j=1; j<nodes.size(); j++){
            string term = buildTransitTerm(1, 0, j, k, " + ");
            addTerm(cordeauConstraint4, term);
        }
        cordeauConstraint4.erase(cordeauConstraint4.size() - operation.size());
        cordeauConstraint4 += " = 1\n";
    }

    return cordeauConstraint4;
}

/*
 * Constraint 5: every pickup and dropoff node (1-2n) must have the same number of inbound and outbound edges
 */
string cordeauConstraint5(vector<Node> nodes, vector<Vehicle> vehicles){
    string cordeauConstraint5;
    string operation = " + ";
    string operation2 = " - ";

    for (unsigned i = 1; i <= nodes.size()-2; i++) {
        for (unsigned k = 0; k < vehicles.size(); k++) {
            for (unsigned j = 0; j < nodes.size(); j++) {
                if (i != j) {
                    string term = buildTransitTerm(1, j, i, k, operation);
                    addTerm(cordeauConstraint5, term);
                }                                        
            }
            cordeauConstraint5.erase(cordeauConstraint5.size() - operation.size());
            cordeauConstraint5 += operation2;
            for (unsigned j = 0; j < nodes.size(); j++) {
                if (i != j) {
                    string term = buildTransitTerm(1, i, j, k, operation2);
                    addTerm(cordeauConstraint5, term);
                }
            }
            cordeauConstraint5.erase(cordeauConstraint5.size() - operation.size());
            cordeauConstraint5 += " = 0\n";
        }
    }
    return cordeauConstraint5;
}

/*
 * Constraint 6: all vehicles end at destination depot
 */
string cordeauConstraint6(vector<Node> nodes, vector<Vehicle> vehicles){
    string cordeauConstraint6;
    string operation = " + ";

    for(unsigned k = 0; k < vehicles.size(); k++){
        for(unsigned i=0; i < nodes.size(); i++){
            string term = buildTransitTerm(1, i, nodes.size(), k, operation);
            addTerm(cordeauConstraint6, term);
        }
        cordeauConstraint6.erase(cordeauConstraint6.size() - operation.size());
        cordeauConstraint6 += " = 1\n";
    }
    return cordeauConstraint6;
}

/*
 * Constraint 7: service at node j (destination) has to start after service at node i (corresponding origin) is finished and after the vehicle has driven from i to j
 */
string cordeauConstraint7(vector<Node> nodes, vector<Vehicle> vehicles){
    string cordeauConstraint7;
    string addNextTerm = " + ";
    string subtractNextTerm = " - ";
    for(unsigned i=0; i < nodes.size(); i++){
        for(unsigned j=0; j < nodes.size(); j++){
            if(i != j){
                for(unsigned k=0; k < vehicles.size(); k++){
                    int m = nodes[i].latestServiceTime + nodes[i].serviceDuration + getTransitTime(nodes[i], nodes[j]) - nodes[j].earliestServiceTime;
                    if(m<0){
                        m=0;
                    }
                    string term = buildStartServiceTerm(1, i, k, subtractNextTerm);
                    addTerm(cordeauConstraint7, term);

                    term = buildStartServiceTerm(1, j, k, addNextTerm);
                    addTerm(cordeauConstraint7, term);
                    
                    term = buildTransitTerm(m, i, j, k, addNextTerm);
                    addTerm(cordeauConstraint7, term);

                    cordeauConstraint7.erase(cordeauConstraint7.size() - 3);
                    cordeauConstraint7 += " <= ";
                    int constant = (nodes[i].serviceDuration + getTransitTime(nodes[i], nodes[j]) - m) * -1;
                    cordeauConstraint7 += to_string(constant);
                    cordeauConstraint7 += "\n";
                }
            }
        }
    }
    return cordeauConstraint7;
}

string cordeauConstraint8(vector<Node> nodes, vector<Vehicle> vehicles){
    string CordeauConstraint8;

    return CordeauConstraint8;
}


//boilerplate for Spyglass
int getTransitTime(Node i, Node j){
    return 10;
}

void addTerm(string &constraint, string term){
    constraint += term;
    int currentLineSize = constraint.size() - constraint.rfind("\n");
    if(currentLineSize > 200){
        constraint += "\n";
    }
}

string buildConstantTerm(int constant, string operation){
    string term;
    
    term += to_string(constant);
    term += operation;

    return term;
}

string buildStartServiceTerm(int coefficient, int nodeIndex, int vehicle, string operation){
    string term;

    term += to_string(coefficient);
    term += " b";
    term += to_string(nodeIndex);
    term += to_string(vehicle);
    term += operation;

    return term;
}

string buildTransitTerm(int coefficient, int origin, int destination, int vehicle, string operation){
    string term;

    term += to_string(coefficient);
    term += " x";
    term += to_string(origin);
    term += to_string(destination);
    term += to_string(vehicle);
    term += operation;

    return term;
}
