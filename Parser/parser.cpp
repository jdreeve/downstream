#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#define REQUIREMENTS_FILE "./requirements.csv"
#define VEHICLES_FILE "./vehicles.csv"
#define LP_FILE "requirements.lp"

using namespace std;

class Node{
    public:
        string address;
        double xPosition;
        double yPosition;
        double earliestServiceTime;
        double latestServiceTime;
        int load;

        Node(const string addr, double x, double y, double early, double late, int ld){
            address = addr;
            xPosition = x;
            yPosition = y;
            earliestServiceTime = early;
            latestServiceTime = late;
            load = ld;
        }
};

void parseRequirements(vector<Node> &nodes);
void getVehicles(vector<Vehicles> &vehicles);
void writeLPFile(std::string filePath, vector<Node> nodes);
string getObjective(vector<Node> nodes);
void writeObjective(std::string filePath);
void writeConstraints(std::string filePath);
void writeBinarySection(std::string filePath);
void writeTerminal(std::string filePath);

int main() {
    vector<Node> nodes;
    vector<Vehicle> vehicles;
    parseRequirements(nodes);
    getVehicles(vehicles);
    writeLPFile(LP_FILE, nodes);
}

void parseRequirements(vector<Node> &nodes){
    vector<Node> origins;
    vector<Node> destinations;
    //vector<Client> client //stores current locations of clients
    //go through file to create origins and destinations
    //
    //nodes.push_back(FIRST) //add origin node
    //nodes.insert(nodes.end(), origins.begin(), origins.end());
    //nodes.insert(nodes.end(), destinations.begin(), destinations.end());
    //nodes.push_back(FIRST) //add destination node
}

void writeLPFile(std::string filePath, vector<Node> nodes, vector<Vehicle> vehicles){
    string objective;
    objective = getObjective(nodes);
    writeObjective(filePath);
    writeConstraints(filePath);
    writeBinarySection(filePath);
    writeTerminal(filePath);
}

string getObjective(vector<Node> nodes, vector<Vehicle> vehicles){
    string objective;
    for(unsigned i=0; i< nodes.size(); i++){
        for(unsigned j = 0; j < nodes.size(); j++){
            if(i!=j){
                for(int )
            }
        }
    }

    return objective;
}


void writeObjective(std::string filePath){
    ofstream lpfile;
    lpfile.open(filePath);
    lpfile << "MINIMIZE\n";



    lpfile.close();
}

void writeConstraints(std::string filePath){
    ofstream lpfile;
    lpfile.open(filePath, ios_base::app);
    lpfile << "SUBJECT TO\n";
    lpfile.close();
}

void writeBinarySection(std::string filePath){
    ofstream lpfile;
    lpfile.open(filePath, ios_base::app);
    lpfile << "BINARY\n";
    lpfile.close();
}

void writeTerminal(std::string filePath){
    ofstream lpfile;
    lpfile.open(filePath, ios_base::app);
    lpfile << "END";
    lpfile.close();
}
