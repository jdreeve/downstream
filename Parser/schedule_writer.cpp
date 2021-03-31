#include <iostream>
#include <fstream>
#include "schedule_writer.hpp"
#include "edge.hpp"

void ScheduleWriter::buildRoutes(){
    ifstream solutionFile(solutionPath);
    string line;

    //skip first line
    getline(solutionFile, line);

    while(getline(solutionFile, line)){
        processSolutionLine(line);
    }

    solutionFile.close();

    //open solution file
    //skip first line
    //for each line:
    //  skip initial spaces, variable number, space
    //  if first character is x:
    //      originIndex after i, destIndex after j, vehicleIndex after k
    //      find Route of vehicle in routes
    //      if not found
    //          create Route, push back
    //      add edge to Route
    //  if first character is b:
    //      nodeIndex after i, vehicleIndex after k
    //      update edge in route
}

void ScheduleWriter::processSolutionLine(string line){
    int iterator = 0;
    bool merged = false;

    //skip initial spaces
    while(line[iterator] == ' '){
        iterator++;
    }
    //skip solver variable ID number
    while(line[iterator] != ' '){
        iterator++;
    }

    //skip space separator
    iterator++;

    if(line[iterator] == 'x'){
        int tokenLength = 0;
        while(line[iterator+tokenLength] != ' '){
            tokenLength++;
        }
        string token = line.substr(iterator, tokenLength);
        processSolutionEdge(token);
    }

    if(line[iterator] == 'b'){
        if(!merged){
            mergeRoutes();
            merged = true;
        }
        double value = 0;
        int tokenLength = 0;
        while(line[iterator+tokenLength] != ' '){
            tokenLength++;
        }
        string timeToken = line.substr(iterator, tokenLength);

        iterator += tokenLength;
        while(line[iterator] == ' '){
            iterator++;
        }
        tokenLength = 0;
        while(line[iterator+tokenLength+1] != ' '){
            tokenLength++;
        }
        string valueToken = line.substr(iterator, tokenLength);
        value = stod(valueToken);

        processSolutionTime(timeToken, value);
    }
}

void ScheduleWriter::processSolutionEdge(string token){
    int iterator = 0;
    string numString = "";
    int originNodeIndex;
    int destinationNodeIndex;
    int vehicleIndex;

    //skip x and i characters
    iterator +=2;

    //get origin node index
    while(token[iterator] != 'j'){
        numString.push_back(token[iterator]);
        iterator++;
    }

    originNodeIndex = stoi(numString);

    //get destination node index
    iterator++;
    numString = "";
    while(token[iterator] != 'k'){
        numString.push_back(token[iterator]);
        iterator++;
    }

    destinationNodeIndex = stoi(numString);

    //get vehicle number
    iterator++;
    numString = "";
    while(token[iterator] != ' '){
        numString.push_back(token[iterator]);
        iterator++;
    }

    vehicleIndex = stoi(numString);

    Edge edge = Edge(nodes[originNodeIndex], nodes[destinationNodeIndex]);

    //find route with same vehicle

    int routeIndex = -1;
    for(unsigned i=0; i < this->routes.size(); i++){
        if (routes[i].vehicleID == this->vehicles[vehicleIndex].ID){
            routeIndex = i;
            break;
        }
    }
    if(-1 == routeIndex){
        Route route = Route();
        route.edges.push_back(edge);
        route.vehicleID = this->vehicles[vehicleIndex].ID;
        routes.push_back(route);
    }
    else{
        int edgeIndex = -1;
        for(unsigned i=0; i < routes[routeIndex].edges.size(); i++){
            if(routes[routeIndex].edges[i].getDestination().nodeID == edge.getOrigin().nodeID){
                vector<Edge>::iterator it;
                it = routes[routeIndex].edges.begin();
                it += i;
                it +=1;
                routes[routeIndex].edges.insert(it, edge);
                edgeIndex = i+1;
            }
        }
        if(-1 == edgeIndex){
        Route route = Route();
        route.edges.push_back(edge);
        route.vehicleID = this->vehicles[vehicleIndex].ID;
        routes.push_back(route);
        }
    }
}

void ScheduleWriter::mergeRoutes(){
    int i=0;
    int currentVehicleID;
    while(this->routes.size() > this->vehicles.size()){
        currentVehicleID = this->vehicles[i].ID;
        bool mergeFound = false;
        unsigned j;
        //find first route segment with currentVehicleID
        for(j=0; j< this->routes.size(); j++){
            if (routes[j].vehicleID == currentVehicleID){
                break;
            }
        }
        //find merge target
        Node mergeTail = this->routes[j].edges[this->routes[j].edges.size()-1].getDestination();
        for(unsigned k = j+1; k < this->routes.size(); k++){
            Node routeHead = routes[k].edges[routes[k].edges.size()-1].getOrigin();
            if( mergeTail.nodeID == routeHead.nodeID && (routes[j].vehicleID == routes[k].vehicleID)){
                //write routes[i].edges onto mainRoute.edges
                this->routes[j].edges.insert(this->routes[j].edges.end(), routes[k].edges.begin(), routes[k].edges.end());
                this->routes.erase(routes.begin()+k);
                mergeFound = true;
                break;
            }
        }
        if(!mergeFound){
            i++;
        }
    }
}

void ScheduleWriter::printRoutes(){
    for(unsigned i=0; i < this->routes.size(); i++){
        printRoute(i);
    }
}

void ScheduleWriter::printRoute(unsigned index){
    cout << "Printing route for vehicle " << this->routes[index].vehicleID << "\n";
    for(unsigned i=0; i < this->routes[index].edges.size(); i++){
        routes[index].edges[i].printEdge();
    }
    cout << "\n\n";
}

void ScheduleWriter::processSolutionTime(string token, double value){
    int originIndex;
    int originID;
    int vehicleIndex;
    int vehicleID;
    int iterator;

    string numString = "";
    iterator = 2;

    //get origin
    while(token[iterator] != 'k'){
        numString.push_back(token[iterator]);
        iterator++;
    }
    originIndex = stoi(numString);
    originID = this->nodes[originIndex].nodeID;


    //get vehicle ID
    iterator++;//skip 'k'
    numString = token.substr(iterator);
    vehicleIndex = stoi(numString);
    vehicleID = this->vehicles[vehicleIndex].ID;

    for(unsigned i=0; i < this->routes.size(); i++){
        if(this->routes[i].vehicleID == vehicleID){
            for(unsigned j=0; j < this->routes[i].edges.size(); j++){
                Edge currentEdge = this->routes[i].edges[j];
                if (currentEdge.getOrigin().nodeID == originID){
                    currentEdge.setDepartureTime(value);
                    break;
                }
            }
        }
    }
}

void ScheduleWriter::writeScheduleFile(string filePath){
    ofstream scheduleFile;
    scheduleFile.open(filePath);

    for(unsigned i=0; i < this->routes.size(); i++){
        Route currentRoute = this->routes[i];
        scheduleFile << "Vehicle: " << currentRoute.vehicleID << "\n";
        for(unsigned j=0; j < currentRoute.edges.size(); j++){
            Edge currentEdge = currentRoute.edges[j];
            scheduleFile << currentEdge.getDepartureTime() << ",";
            scheduleFile << currentEdge.getOrigin().address << ",";
            scheduleFile << currentEdge.getDestination().address << ",";
            scheduleFile << currentEdge.getOrigin().name;
            scheduleFile << "\n";
        }
    }
    scheduleFile.close();
}
