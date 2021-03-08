#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#define DEBUG 1

#define REQUIREMENTS_FILE "./requirements.csv"
#define VEHICLES_FILE "./vehicles.csv"
#define LP_FILE "requirements.lp"
#define DELIMITER ","

#define MAX_LINE 100

#define VEHICLE_COUNT 10
#define VEHICLE_CAPACITY 8
#define VEHICLE_MAX_TIME 20000

#define EARLY_ARRIVAL_WINDOW 10
#define LATE_ARRIVAL_WINDOW 0
#define SERVICE_DURATION 0
#define MAX_WAIT 15
#define DEPOT_ADDRESS "FIRST address goes here"

using namespace std;

class Node{
    public:
        string name;
        int earliestServiceTime;
        int latestServiceTime;
        int serviceDuration;
        string address;
        int load;

        Node(){
            name = "";
            earliestServiceTime = 0;
            latestServiceTime = 1440;
            serviceDuration = 0;
            address = "";
            load = 0;
        }
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
string cordeauConstraint3(vector<Node> nodes, vector<Vehicle> vehicles);
string cordeauConstraint4(vector<Node> nodes, vector<Vehicle> vehicles);
string cordeauConstraint5(vector<Node> nodes, vector<Vehicle> vehicles);
string cordeauConstraint6(vector<Node> nodes, vector<Vehicle> vehicles);
string cordeauConstraint7(vector<Node> nodes, vector<Vehicle> vehicles);
string cordeauConstraint8(vector<Node> nodes, vector<Vehicle> vehicles);
string cordeauConstraint11(vector<Node> nodes, vector<Vehicle> vehicles);
string cordeauConstraint13(vector<Node> nodes, vector<Vehicle> vehicles);
int getTransitTime(Node i, Node j);

string buildConstantTerm(int constant, string operation);
string buildThreeIndexedTerm(int coefficient, int origin, int destination, int vehicle, string operation);
string buildStartServiceTerm(int coefficient, int nodeIndex, int vehicle, string operation);
string buildTwoIndexedTerm(int coefficient, string variable, string index1Variable, int index1Value, string index2Variable, int index2Value);
void writeLengthCappedString(string &constraint, string term);
string buildIndent(int depth);

int main() {
    vector<Node> nodes;
    vector<Vehicle> vehicles;
    parseRequirements(nodes);
    getVehicles(vehicles);
    writeLPFile(LP_FILE, nodes, vehicles);
}

void parseRequirements(vector<Node> &nodes){
    vector<Node> origins;
    vector<Node> destinations;
    string line;
    ifstream requirements(REQUIREMENTS_FILE);
    //add origin depot node
    Node depot = Node("Depot", 0, 1440, 0, DEPOT_ADDRESS);
    nodes.push_back(depot);
    while(getline(requirements, line)){//parse each line
        string delimiter = DELIMITER;
        string eventName;
        int eventStartTime;
        int eventFinishTime;
        double eventLoad;
        string eventAddress;
        size_t tokenStart = 0;
        size_t tokenEnd = 0;
        Node temp = Node();
        
        //get name
        tokenEnd = line.find(delimiter, tokenStart);
        eventName = line.substr(tokenStart, tokenEnd - tokenStart);
        
        //get load
        tokenStart = tokenEnd + delimiter.length();
        tokenEnd = line.find(delimiter, tokenStart);
        eventLoad = stoi(line.substr(tokenStart, tokenEnd - tokenStart));

        //get dropoff time
        tokenStart = tokenEnd + delimiter.length();
        tokenEnd = line.find(delimiter, tokenStart);
        eventStartTime = stoi(line.substr(tokenStart, tokenEnd - tokenStart));

        //get pickup time
        tokenStart = tokenEnd + delimiter.length();
        tokenEnd = line.find(delimiter, tokenStart);
        eventFinishTime = stoi(line.substr(tokenStart, tokenEnd - tokenStart));

        //get address
        tokenStart = tokenEnd + delimiter.length();
        tokenEnd = line.find(delimiter, tokenStart);
        eventAddress = line.substr(tokenStart, line.size());

        //create origin node for trip toEvent
        temp.address = depot.address;
        temp.earliestServiceTime = eventStartTime - getTransitTime(depot, temp) - EARLY_ARRIVAL_WINDOW;
        temp.latestServiceTime = eventStartTime - getTransitTime(depot, temp) + LATE_ARRIVAL_WINDOW;
        temp.serviceDuration = SERVICE_DURATION;
        temp.load = eventLoad;
        temp.name = eventName;

        origins.push_back(temp);

        //create origin node for trip fromEvent
        temp.address = eventAddress;
        temp.earliestServiceTime = eventFinishTime;
        temp.latestServiceTime = eventFinishTime + MAX_WAIT;
        temp.serviceDuration = SERVICE_DURATION;
        temp.load = eventLoad;
        temp.name = eventName;

        origins.push_back(temp);

        //create destination nodes for trip toEvent
        temp.address = eventAddress;
        temp.earliestServiceTime = eventStartTime - EARLY_ARRIVAL_WINDOW;
        temp.latestServiceTime = eventStartTime + LATE_ARRIVAL_WINDOW;
        temp.serviceDuration = SERVICE_DURATION;
        temp.load = eventLoad * -1;
        temp.name = eventName;

        destinations.push_back(temp);

        //origin node for trip fromEvent
        temp.address = depot.address;
        temp.earliestServiceTime = eventFinishTime + getTransitTime(depot, temp);
        temp.latestServiceTime = eventFinishTime + getTransitTime(depot, temp) + MAX_WAIT;
        temp.serviceDuration = SERVICE_DURATION;
        temp.load = eventLoad * -1;
        temp.name = eventName;

        destinations.push_back(temp);
    }
    //write origins onto nodes vector
    nodes.insert(nodes.end(), origins.begin(), origins.end());
    //write destinations onto nodes vector
    nodes.insert(nodes.end(), destinations.begin(), destinations.end());
    //add terminal depot node
    nodes.push_back(depot);

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
    string objective = buildIndent(1);
    string addition = " + ";
    for(unsigned i=0; i< nodes.size(); i++){
        for(unsigned j=0; j < nodes.size(); j++){
            if(i!=j){
                for(unsigned k=0; k<vehicles.size(); k++){
                    string term = buildThreeIndexedTerm(1, i, j, k, addition);
                    writeLengthCappedString(objective, term);
                    }
                }
            }
        }
    size_t lastSpaceIndex = objective.find_last_of(" ");
    objective.erase(lastSpaceIndex - 2);
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
                    tempString += "x";
                    tempString += "i" + to_string(i);
                    tempString += "j" + to_string(j);
                    tempString += "k" + to_string(k);
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
    constraint = cordeauConstraint3(nodes, vehicles);
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
    constraint = cordeauConstraint8(nodes, vehicles);
    lpfile << "\\Constraint 8\n";
    lpfile << constraint;
    lpfile << "\n";
    //constraint 9: max ride time of user
    //constraint 10: max tour time of vehicle
    constraint = cordeauConstraint11(nodes, vehicles);
    lpfile << "\\Constraint 11\n";
    lpfile << constraint;
    lpfile << "\n";
    //constraint 12: travel time constraints
    constraint = cordeauConstraint13(nodes, vehicles);
    lpfile << "\\Constraint 13\n";
    lpfile << constraint;
    lpfile << "\n";

    lpfile.close();
}

/* 
 * Constraint 2: Visit every pickup location (i.e. serve every request exactly once)
 */
string cordeauConstraint2(vector<Node> nodes, vector<Vehicle> vehicles){
    string cordeauConstraint2 = buildIndent(1);
    string addition = " + ";

    for(unsigned i=1; i<=(nodes.size()-2)/2; i++){
        for(unsigned k=0; k<vehicles.size(); k++){
            for(unsigned j=0; j<nodes.size(); j++){
                if(i!=j){
                    string term = buildThreeIndexedTerm(1, i, j, k, addition);
                    writeLengthCappedString(cordeauConstraint2, term);
                }
            }
        }
        cordeauConstraint2.erase(cordeauConstraint2.size()-addition.size());
        cordeauConstraint2 += " = 1\n" + buildIndent(1);
    }
    return cordeauConstraint2;
}

/*
 * Constraint 3: The same vehicle must visit corresponding pickup and dropoff nodes.
 */
string cordeauConstraint3(vector<Node> nodes, vector<Vehicle> vehicles){
    string cordeauConstraint3 = buildIndent(1);
    string operation1 = " + ";
    string operation2 = " - ";
    int charCounter = 0;
    for(unsigned i = 1; i <= (nodes.size()-2)/2; i++){
        for(unsigned k = 0; k < vehicles.size(); k++){
            for(unsigned j = 0; j < nodes.size(); j++){
                if(i != j){
                    string term = buildThreeIndexedTerm(1, i, j, k, " + ");
                    writeLengthCappedString(cordeauConstraint3, term);
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
                        string term = buildThreeIndexedTerm(1, (nodes.size()-2)/2 +i, j, k, " - ");
                        writeLengthCappedString(cordeauConstraint3, term);
                    }
                }
            }
        cordeauConstraint3.erase(cordeauConstraint3.size()-3);
        cordeauConstraint3 += " = 0\n" + buildIndent(1);
        charCounter = 0;
        }
    }
    return cordeauConstraint3;
}

/*
 * Constraint 4: routes begin at origin depot (node 0).
 */
string cordeauConstraint4(vector<Node> nodes, vector<Vehicle> vehicles){
    string cordeauConstraint4 = buildIndent(1);
    string operation = " + ";
    for(unsigned k=0; k<vehicles.size(); k++){
        for(unsigned j=1; j<nodes.size(); j++){
            string term = buildThreeIndexedTerm(1, 0, j, k, " + ");
            writeLengthCappedString(cordeauConstraint4, term);
        }
        cordeauConstraint4.erase(cordeauConstraint4.size() - operation.size());
        cordeauConstraint4 += " = 1\n" + buildIndent(1);
    }

    return cordeauConstraint4;
}

/*
 * Constraint 5: every pickup and dropoff node (1-2n) must have the same number of inbound and outbound edges
 */
string cordeauConstraint5(vector<Node> nodes, vector<Vehicle> vehicles){
    string cordeauConstraint5 = buildIndent(1);
    string operation = " + ";
    string operation2 = " - ";

    for (unsigned i = 1; i <= nodes.size()-2; i++) {
        for (unsigned k = 0; k < vehicles.size(); k++) {
            for (unsigned j = 0; j < nodes.size(); j++) {
                if (i != j) {
                    string term = buildThreeIndexedTerm(1, j, i, k, operation);
                    writeLengthCappedString(cordeauConstraint5, term);
                }                                        
            }
            cordeauConstraint5.erase(cordeauConstraint5.size() - operation.size());
            cordeauConstraint5 += operation2;
            for (unsigned j = 0; j < nodes.size(); j++) {
                if (i != j) {
                    string term = buildThreeIndexedTerm(1, i, j, k, operation2);
                    writeLengthCappedString(cordeauConstraint5, term);
                }
            }
            cordeauConstraint5.erase(cordeauConstraint5.size() - operation.size());
            cordeauConstraint5 += " = 0\n" + buildIndent(1);
        }
    }
    return cordeauConstraint5;
}

/*
 * Constraint 6: all vehicles end at destination depot
 */
string cordeauConstraint6(vector<Node> nodes, vector<Vehicle> vehicles){
    string cordeauConstraint6 = buildIndent(1);
    string operation = " + ";

    for(unsigned k = 0; k < vehicles.size(); k++){
        for(unsigned i=0; i < nodes.size() - 1; i++){
            string term = buildThreeIndexedTerm(1, i, nodes.size()-1, k, operation);
            writeLengthCappedString(cordeauConstraint6, term);
        }
        cordeauConstraint6.erase(cordeauConstraint6.size() - operation.size());
        cordeauConstraint6 += " = 1\n" + buildIndent(1);
    }
    return cordeauConstraint6;
}

/*
 * Constraint 7: service at node j (destination) has to start after service at node i (corresponding origin) is finished and after the vehicle has driven from i to j
 */
string cordeauConstraint7(vector<Node> nodes, vector<Vehicle> vehicles){
    string cordeauConstraint7 = buildIndent(1);
    string addNextTerm = " + ";
    string variable = "b";
    int coefficient = 1;
    string subtractNextTerm = " - ";
    for(unsigned i=0; i < nodes.size(); i++){
        for(unsigned j=0; j < nodes.size(); j++){
            if(i != j){
                for(unsigned k=0; k < vehicles.size(); k++){
                    int m = nodes[i].latestServiceTime + nodes[i].serviceDuration + getTransitTime(nodes[i], nodes[j]) - nodes[j].earliestServiceTime;
                    if(m<0){
                        m=0;
                    }
                    string term;

                    term = buildTwoIndexedTerm(coefficient, variable, "i", i, "k", k);
                    writeLengthCappedString(cordeauConstraint7, term);

                    writeLengthCappedString(cordeauConstraint7, " - ");

                    term = buildTwoIndexedTerm(coefficient, variable, "i", j, "k", k);
                    writeLengthCappedString(cordeauConstraint7, term);
                    
                    writeLengthCappedString(cordeauConstraint7, " + ");
                    
                    term = buildThreeIndexedTerm(m, i, j, k, addNextTerm);
                    writeLengthCappedString(cordeauConstraint7, term);

                    cordeauConstraint7.erase(cordeauConstraint7.size() - 3);
                    cordeauConstraint7 += " <= ";
                    int constant = (nodes[i].serviceDuration + getTransitTime(nodes[i], nodes[j]) - m) * -1;
                    cordeauConstraint7 += to_string(constant);
                    cordeauConstraint7 += "\n" + buildIndent(1);
                }
            }
        }
    }
    return cordeauConstraint7;
}

//Constraint 8: load on vehicle k at node i + load of node j <= k.capacity
string cordeauConstraint8(vector<Node> nodes, vector<Vehicle> vehicles){
    string cordeauConstraint8 = buildIndent(1);
    string term;
    int w;
    int coefficient = 1;
    string variable = "q";
    for(unsigned i=0; i<nodes.size(); i++){
        for(unsigned j=0; j<nodes.size(); j++){
            if(i != j){
                for(unsigned k=0; k<vehicles.size(); k++){
                    w = vehicles[k].capacity + nodes[i].load;
                    if(w>vehicles[k].capacity){
                        w = vehicles[k].capacity;
                    }

                    term = buildTwoIndexedTerm(coefficient, variable, "i", i, "k", k);
                    writeLengthCappedString(cordeauConstraint8, term);
                    writeLengthCappedString(cordeauConstraint8, " - ");

                    term = buildTwoIndexedTerm(coefficient, variable, "i", j, "k", k);
                    writeLengthCappedString(cordeauConstraint8, term);
                    writeLengthCappedString(cordeauConstraint8, " + ");

                    term = buildThreeIndexedTerm(w, i, j, k, "");
                    writeLengthCappedString(cordeauConstraint8, term);

                    writeLengthCappedString(cordeauConstraint8, " <= ");

                    int rightSide = w - nodes[j].load;
                    term = buildConstantTerm(rightSide, "");
                    writeLengthCappedString(cordeauConstraint8, term);

                    term = "\n" + buildIndent(1);
                    writeLengthCappedString(cordeauConstraint8, term);
                }
            }
        }
    }

    return cordeauConstraint8;
}

//Constraint 11: nodes must be visited within their service time.
string cordeauConstraint11(vector<Node> nodes, vector<Vehicle> vehicles){
    string cordeauConstraint11 = buildIndent(1);
    int coefficient = 1;
    string variable = "b";

    for(unsigned i=0; i<nodes.size(); i++){
        for(unsigned k=0; k<vehicles.size(); k++){
            string term;

            int earlyService = nodes[i].earliestServiceTime;
            int lateService = nodes[i].latestServiceTime;

            term = buildTwoIndexedTerm(coefficient, variable, "i", i, "k", k);
            writeLengthCappedString(cordeauConstraint11, term);

            writeLengthCappedString(cordeauConstraint11, " >= ");

            term = buildConstantTerm(earlyService, "");
            writeLengthCappedString(cordeauConstraint11, term);

            term = "\n" + buildIndent(1);
            writeLengthCappedString(cordeauConstraint11, term);

            term = buildTwoIndexedTerm(coefficient, variable, "i", i, "k", k);
            writeLengthCappedString(cordeauConstraint11, term);

            writeLengthCappedString(cordeauConstraint11, " <= ");

            term = buildConstantTerm(lateService, "");
            writeLengthCappedString(cordeauConstraint11, term);

            term = "\n" + buildIndent(1);
            writeLengthCappedString(cordeauConstraint11, term);
        }
    }

    return cordeauConstraint11;
}

//Constraint 13: vehicles must not exceed max load.
string cordeauConstraint13(vector<Node> nodes, vector<Vehicle> vehicles){
    string cordeauConstraint13 = buildIndent(1);
    string variable = "q";
    int coefficient = 1;

    for(unsigned i=0; i < nodes.size(); i++){
        for(unsigned k=0; k < vehicles.size(); k++){
            string term;
            int q;

            q = vehicles[k].capacity;
            if(nodes[i].load < 0){
                q += nodes[i].load;
            }

            term = buildTwoIndexedTerm(coefficient, variable, "i", i, "k", k);
            writeLengthCappedString(cordeauConstraint13, term);

            writeLengthCappedString(cordeauConstraint13, " <= ");

            term = buildConstantTerm(q, "");

            writeLengthCappedString(cordeauConstraint13, term);

            term = "\n" + buildIndent(1);
            writeLengthCappedString(cordeauConstraint13, term);


            q = nodes[i].load;
            if(q<0){
                q=0;
            }

            term = buildTwoIndexedTerm(coefficient, variable, "i", i, "k", k);
            writeLengthCappedString(cordeauConstraint13, term);

            writeLengthCappedString(cordeauConstraint13, " >= ");

            term = buildConstantTerm(q, "");

            writeLengthCappedString(cordeauConstraint13, term);

            term = "\n" + buildIndent(1);
            writeLengthCappedString(cordeauConstraint13, term);
        }
    }
    return cordeauConstraint13;
}

//boilerplate for Spyglass
int getTransitTime(Node i, Node j){
    return 10;
}


void writeLengthCappedString(string &constraint, string augment){
    constraint += augment;
    int currentLineSize = constraint.size() - constraint.rfind("\n");
    if(currentLineSize > MAX_LINE){
        constraint += "\n" + buildIndent(2);
    }
}

string buildIndent(int depth){
    string indent;
    for(int i=0; i < depth; i++){
        indent += "    ";
    }
    return indent;
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

string buildTwoIndexedTerm(int coefficient, string variable, string index1Variable, int index1Value, string index2Variable, int index2Value){
    string term;

    term += to_string(coefficient);
    term += " ";
    term += variable;
    term += index1Variable + to_string(index1Value);
    term += index2Variable + to_string(index2Value);

    return term;
}

string buildThreeIndexedTerm(int coefficient, int origin, int destination, int vehicle, string operation){
    string term;

    term += to_string(coefficient);
    term += " x";
    term += "i" + to_string(origin);
    term += "j" + to_string(destination);
    term += "k" + to_string(vehicle);
    term += operation;
    return term;
}
