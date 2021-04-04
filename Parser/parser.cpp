#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "node.hpp"
#include "vehicle.hpp"
#include "parser.hpp"
#include "settings.hpp"
#include "csv.hpp"
#include "spyglass.hpp"

#define DEBUG 1

#define DELIMITER ","

#define MAX_LINE 100

#define VEHICLE_MAX_TIME 200000

#define DEPOT_ADDRESS "32 Knox Rd Ridgecrest NC 28770"

using namespace std;

void Parser::parseRequirements(string requirementsPath){
    string line;
    vector<Node> origins;
    vector<Node> destinations;
    ifstream requirements(requirementsPath);
    //add origin depot node
    Node depot = Node("Depot", 0, 1440, 0, this->settings.getVehicleDepotAddress(), 0);
    this->nodes.push_back(depot);

    string eventName;
    int eventLoad;
    int eventStartTimeIn24;
    int eventFinishTimeIn24;
    string eventAddress;
   
    io::CSVReader<5> requirementsCSVReader(requirementsPath);
    while(requirementsCSVReader.read_row(eventName, eventLoad, eventStartTimeIn24, eventFinishTimeIn24, eventAddress)){
        int eventStartTime;
        int eventFinishTime;
        Node temp = Node();

        eventStartTime = convert24HourTimeToMinutes(eventStartTimeIn24);
        eventFinishTime = convert24HourTimeToMinutes(eventFinishTimeIn24);

        //create origin node for trip toEvent
        temp.address = depot.address;
        int transitTime = getTransitTime(depot, temp);
        temp.earliestServiceTime = eventStartTime - transitTime- this->settings.getEarlyArrivalWindow();
        temp.latestServiceTime = eventStartTime - transitTime + this->settings.getLateArrivalWindow();
        temp.serviceDuration = this->settings.getServiceDuration();
        temp.load = eventLoad;
        temp.name = eventName;
        origins.push_back(temp);

        //create origin node for trip fromEvent
        temp.address = eventAddress;
        temp.earliestServiceTime = eventFinishTime;
        temp.latestServiceTime = eventFinishTime + this->settings.getMaxWait();
        temp.serviceDuration = this->settings.getServiceDuration();
        temp.load = eventLoad;
        temp.name = eventName;
        origins.push_back(temp);

        //create destination node for trip toEvent
        temp.address = eventAddress;
        temp.earliestServiceTime = eventStartTime - this->settings.getEarlyArrivalWindow();
        temp.latestServiceTime = eventStartTime + this->settings.getLateArrivalWindow();
        temp.serviceDuration = this->settings.getServiceDuration();
        temp.load = eventLoad * -1;
        temp.name = eventName;
        destinations.push_back(temp);

        //create destination node for trip fromEvent
        temp.address = depot.address;
        temp.earliestServiceTime = eventFinishTime + transitTime;
        temp.latestServiceTime = eventFinishTime + transitTime + this->settings.getMaxWait();
        temp.serviceDuration = this->settings.getServiceDuration();
        temp.load = eventLoad * -1;
        temp.name = eventName;
        destinations.push_back(temp);
    }

    //write origins onto nodes vector
    this->nodes.insert(nodes.end(), origins.begin(), origins.end());
    //write destinations onto nodes vector
    this->nodes.insert(nodes.end(), destinations.begin(), destinations.end());
    //add terminal depot node
    this->nodes.push_back(depot);

    for(unsigned i=0; i<nodes.size(); i++){
        nodes[i].nodeID = i;
    }

    requirements.close();
    //vector<Client> client //stores current locations of clients
    //go through file to create origins and destinations
    //
    //nodes.push_back(FIRST) //add origin node
    //nodes.insert(nodes.end(), origins.begin(), origins.end());
    //nodes.insert(nodes.end(), destinations.begin(), destinations.end());
    //nodes.push_back(FIRST) //add destination node
}

int Parser::convert24HourTimeToMinutes(int timeIn24Hr){
    int minutes;
    int minutesFrom24HrHours;
    int minutesFrom24HrMinutes;
    int offsetTimeIn24Hr = this->addTimeOffset(timeIn24Hr);
    minutesFrom24HrHours = (offsetTimeIn24Hr / 100) * 60;
    minutesFrom24HrMinutes = (offsetTimeIn24Hr % 100);
    minutes = minutesFrom24HrHours + minutesFrom24HrMinutes;

    return minutes;
}

int Parser::addTimeOffset(int timeIn24Hr){
    int timeWithOffset = timeIn24Hr + 200;
    return timeWithOffset;
}

void Parser::printNodes(){
    cout << "<<<<Printing nodes>>>>\n";
    for(unsigned i=0; i < this->nodes.size(); i++){
        cout << "Name: " << this->nodes[i].name << "\n";
        cout << "Early: " << this->nodes[i].earliestServiceTime << "\n";
        cout << "Late: " << this->nodes[i].latestServiceTime << "\n";
        cout << "Load: " << this->nodes[i].load << "\n";
        cout << "Address: " << this->nodes[i].address << "\n";
    }
    cout << "Total nodes: " << this->nodes.size() << "\n";
}

void Parser::getVehicles(string vehiclePath){
    cout << "Getting vehicles\n";
    io::CSVReader<2> vehicleCSVReader(vehiclePath);
    string vehicleName;
    int capacity;
    int vehicleID = -1;
    while(vehicleCSVReader.read_row(vehicleName, capacity)){
        vehicleID++;
        Vehicle temp(vehicleName, capacity, VEHICLE_MAX_TIME);
        temp.ID = vehicleID;
        this->vehicles.push_back(temp);
        cout << "Vehicle created\n";
    }


/*
    ifstream vehicleFile(vehiclePath);
    string line;
    int vehicleID = 0;
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
        temp.ID = vehicleID;
        vehicleID++;
        vehicles.push_back(temp);

    }*/
}

vector<Vehicle> Parser::getParsedVehicles(){
    return this->vehicles;
}

vector<Node> Parser::getParsedNodes(){
    return this->nodes;
}

void Parser::printVehicles(){
    cout << "<<<<Printing vehicles>>>>\n";
    for(unsigned i=0; i < vehicles.size(); i++){
        cout << "Vehicle name: " << vehicles[i].name << "\n";
        cout << "Capacity: " << vehicles[i].capacity << "\n";
        cout << "Max tour time: " << vehicles[i].maxTourTime<< "\n";
    }
    cout << "Total vehicles: " << vehicles.size() << "\n";
}

void Parser::writeLPFile(std::string filePath){
    cout << "Writing objective\n";
    writeObjective(filePath);
    cout << "Writing constraints\n";
    writeConstraints(filePath);
    cout << "Writing binaries\n";
    writeBinarySection(filePath);
    cout << "Writing terminal section\n";
    writeTerminal(filePath);
}

string Parser::getObjective(){
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

string Parser::getBinaryVars(){
    string binaries;

    int charCounter = 0;
    for(unsigned i=0; i< this->nodes.size(); i++){
        for(unsigned j=0; j < nodes.size(); j++){
            if(i!=j){
                for(unsigned k=0; k<this->vehicles.size(); k++){
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

void Parser::writeObjective(string filePath){
    string objective;
    objective = getObjective();
    ofstream lpfile;
    lpfile.open(filePath);
    lpfile << "MINIMIZE\n";
    lpfile << objective;
    lpfile << "\n";
    lpfile.close();
}

void Parser::writeBinarySection(string filePath){
    string binaries = getBinaryVars();
    ofstream lpfile;
    lpfile.open(filePath, ios_base::app);
    lpfile << "BINARY\n";
    lpfile << binaries;
    lpfile << "\n";
    lpfile.close();
}

void Parser::writeTerminal(string filePath){
    ofstream lpfile;
    lpfile.open(filePath, ios_base::app);
    lpfile << "END";
    lpfile.close();
}

void Parser::writeConstraints(string filePath){
    string constraint;
    ofstream lpfile;
    lpfile.open(filePath, ios_base::app);
    lpfile << "SUBJECT TO\n";
    cout << "Writing constraint 2\n";
    constraint = cordeauConstraint2();
    lpfile << "\\Constraint 2\n";
    lpfile << constraint;
    lpfile << "\n";
    cout << "Writing constraint 3\n";
    constraint = cordeauConstraint3();
    lpfile << "\\Constraint 3\n";
    lpfile << constraint;
    lpfile << "\n";
    cout << "Writing constraint 4\n";
    constraint = cordeauConstraint4();
    lpfile << "\\Constraint 4\n";
    lpfile << constraint;
    lpfile << "\n";
    cout << "Writing constraint 5\n";
    constraint = cordeauConstraint5();
    lpfile << "\\Constraint 5\n";
    lpfile << constraint;
    lpfile << "\n";
    cout << "Writing constraint 6\n";
    constraint = cordeauConstraint6();
    lpfile << "\\Constraint 6\n";
    lpfile << constraint;
    lpfile << "\n";
    cout << "Writing constraint 7\n";
    constraint = cordeauConstraint7();
    lpfile << "\\Constraint 7\n";
    lpfile << constraint;
    lpfile << "\n";
    cout << "Writing constraint 8\n";
    constraint = cordeauConstraint8();
    lpfile << "\\Constraint 8\n";
    lpfile << constraint;
    lpfile << "\n";
    //constraint 9: max ride time of user
    //constraint 10: max tour time of vehicle
    cout << "Writing constraint 11\n";
    constraint = cordeauConstraint11();
    lpfile << "\\Constraint 11\n";
    lpfile << constraint;
    lpfile << "\n";
    //constraint 12: travel time constraints
    cout << "Writing constraint 13\n";
    constraint = cordeauConstraint13();
    lpfile << "\\Constraint 13\n";
    lpfile << constraint;
    lpfile << "\n";

    lpfile.close();
}

/* 
 * Constraint 2: Visit every pickup location (i.e. serve every request exactly once)
 */
string Parser::cordeauConstraint2(){
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
string Parser::cordeauConstraint3(){
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
string Parser::cordeauConstraint4(){
    string cordeauConstraint4 = buildIndent(1);
    string operation = " + ";
    for(unsigned k=0; k<this->vehicles.size(); k++){
        for(unsigned j=1; j<this->nodes.size(); j++){
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
string Parser::cordeauConstraint5(){
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
string Parser::cordeauConstraint6(){
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
string Parser::cordeauConstraint7(){
    string cordeauConstraint7 = buildIndent(1);
    string addNextTerm = " + ";
    string variable = "b";
    int coefficient = 1;
    string subtractNextTerm = " - ";
    for(unsigned i=0; i < nodes.size(); i++){
        for(unsigned j=0; j < nodes.size(); j++){
            if(i != j){
                int transitTime = getTransitTime(nodes[i], nodes[j]);
                for(unsigned k=0; k < vehicles.size(); k++){
                    int m = nodes[i].latestServiceTime + nodes[i].serviceDuration + transitTime - nodes[j].earliestServiceTime;
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
                    int constant = (nodes[i].serviceDuration + transitTime - m) * -1;
                    cordeauConstraint7 += to_string(constant);
                    cordeauConstraint7 += "\n" + buildIndent(1);
                    cout << "Nodes count: " << nodes.size() << "\n";
                    cout << "Vehicles count: " << vehicles.size() << "\n";
                    cout << "i: " << i << "  j: " << j << "  k: " << k << "\n";
                    cout << "         ******************       \n";
                }
            }
        }
    }
    return cordeauConstraint7;
}

//Constraint 8: load on vehicle k at node i + load of node j <= k.capacity
string Parser::cordeauConstraint8(){
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
string Parser::cordeauConstraint11(){
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
string Parser::cordeauConstraint13(){
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
int Parser::getTransitTime(Node i, Node j){
    cout << "Creating Spyglass\n";
    Spyglass spyglass = Spyglass(i.address, j.address, this->config);
    int travelTime = spyglass.getTravelTime();
    cout << "Returning " << travelTime << "\n";
    return travelTime;
}


void Parser::writeLengthCappedString(string &constraint, string augment){
    constraint += augment;
    int currentLineSize = constraint.size() - constraint.rfind("\n");
    if(currentLineSize > MAX_LINE){
        constraint += "\n" + buildIndent(2);
    }
}

string Parser::buildIndent(int depth){
    string indent;
    for(int i=0; i < depth; i++){
        indent += "    ";
    }
    return indent;
}

string Parser::buildConstantTerm(int constant, string operation){
    string term;
    
    term += to_string(constant);
    term += operation;

    return term;
}

string Parser::buildStartServiceTerm(int coefficient, int nodeIndex, int vehicle, string operation){
    string term;

    term += to_string(coefficient);
    term += " b";
    term += to_string(nodeIndex);
    term += to_string(vehicle);
    term += operation;

    return term;
}

string Parser::buildTwoIndexedTerm(int coefficient, string variable, string index1Variable, int index1Value, string index2Variable, int index2Value){
    string term;

    term += to_string(coefficient);
    term += " ";
    term += variable;
    term += index1Variable + to_string(index1Value);
    term += index2Variable + to_string(index2Value);

    return term;
}

string Parser::buildThreeIndexedTerm(int coefficient, int origin, int destination, int vehicle, string operation){
    string term;

    term += to_string(coefficient);
    term += " x";
    term += "i" + to_string(origin);
    term += "j" + to_string(destination);
    term += "k" + to_string(vehicle);
    term += operation;
    return term;
}
