#include <iostream>
#include <fstream>
#include "schedule_writer.hpp"
#include "edge.hpp"

void ScheduleWriter::buildRoutes(){
    cout << "Loading solution file from path: " << solutionPath;
    ifstream solutionFile(solutionPath);
    string line;

    //skip first line
    getline(solutionFile, line);

    cout << "First line read\n";
    int lineCounter = 0;
    while(getline(solutionFile, line)){
        cout << "Reading line: " << lineCounter << "\n";
        processSolutionLine(line);
        lineCounter++;
        cout << "Line read: " << lineCounter << "\n";
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
        cout << "X variant\n";
        int tokenLength = 0;
        while(line[iterator+tokenLength] != ' '){
            tokenLength++;
        }
        string token = line.substr(iterator, tokenLength);
        cout << "Token = " << token << "\n";
        processSolutionEdge(token);
    }

    if(line[iterator] == 'b'){
        cout << "B variant\n";
        if(!merged){
            cout << "Preparing to merge routes\n";
            mergeRoutes();
            merged = true;
            cout << "Routes merged\n";
        }
        double value = 0;
        int tokenLength = 0;
        while(line[iterator+tokenLength] != ' '){
            tokenLength++;
        }
        string timeToken = line.substr(iterator, tokenLength);
        cout << "Time token: " << timeToken << "\n";

        iterator += tokenLength;
        while(line[iterator] == ' '){
            iterator++;
        }
        tokenLength = 0;
        while(line[iterator+tokenLength+1] != ' '){
            tokenLength++;
        }
        string valueToken = line.substr(iterator, tokenLength);
        cout << "Value token: " << valueToken << "\n";
        value = stod(valueToken);


        cout << "Calling processSolutionTime with token, value: " << timeToken << " " << value << "\n";
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

        //for each vehicle, merge all routes
        for(unsigned i=0; i < this->vehicles.size(); i++){
            cout << "Iterating for VID " << vehicles[i].ID << "\n";
            cout << "Route count: " << this->routes.size() << "\n";
            cout << "Vehicle count: " << this->vehicles.size() << "\n";
            cout << "VIDs: \n";
            for(unsigned i=0; i<this->routes.size(); i++){
                cout << this->routes[i].vehicleID << "  ";
            }
            cout << "\n";
            cout << "Route heads:\n";
            for(unsigned i=0; i < this->routes.size(); i++){
                Route thisRoute = this->routes[i];
                Edge firstEdge = thisRoute.edges[0];
                Node routeHead = firstEdge.getOrigin();
                Edge lastEdge = thisRoute.edges[thisRoute.edges.size() -1];
                Node routeTail = lastEdge.getDestination();
                cout << "VID: " << thisRoute.vehicleID << "  ";
                cout << "Route head: " << routeHead.nodeID << "  ";
                cout << "Route tail: " << routeTail.nodeID << "\n";
            }

            int currentVehicleID = this->vehicles[i].ID;
            vector<int> matches;
            //find first route with matching VID
            for(unsigned j=0; j < this->routes.size(); j++){
                if (routes[j].vehicleID == currentVehicleID){
                    matches.push_back(j);
                    cout << "VID match at index " << j << "\n";
                    break;
                }
            }
            //find tail of first route
            int firstRouteIndex = matches[0];
            Route firstRoute = this->routes[firstRouteIndex];
            Edge lastEdge = firstRoute.edges[firstRoute.edges.size() -1];
            Node mergeTail = lastEdge.getDestination();
            //merge all routes with matching VID
            /*
            for(unsigned matchesIndex = 1; matchesIndex < matches.size(); matchesIndex++){
                int routeIndex = matches[matchesIndex];
                Route thisRoute = this->routes[routeIndex];
                Node mergeHead = thisRoute.edges[0].getOrigin();
                if(mergeHead.nodeID == mergeTail.nodeID){
                    firstRoute.edges.insert(firstRoute.edges.end(), thisRoute.edges.begin(), thisRoute.edges.end());
                    this->routes.erase(this->routes.begin() + routeIndex);
                    mergeTail = firstRoute.edges[firstRoute.edges.size()-1].getDestination();
                }
            }
            
            */
            
            for(auto thisRoute = routes.begin()+firstRouteIndex+1; thisRoute != routes.end(); thisRoute++){
                if(thisRoute->vehicleID != currentVehicleID){
                    continue;
                }
                Node mergeHead = thisRoute->edges[0].getOrigin();
                if (mergeHead.nodeID == mergeTail.nodeID){
                    cout << "Writing to first route " << firstRouteIndex << "\n";
                    cout << "Length before write: " << firstRoute.edges.size() << "\n";
                    this->routes[firstRouteIndex].edges.insert(this->routes[firstRouteIndex].edges.end(), thisRoute->edges.begin(), thisRoute->edges.end());
                    cout << "Length after write: " << firstRoute.edges.size() << "\n";
                    cout << "Tail after write: " << firstRoute.edges[firstRoute.edges.size()-1].getDestination().nodeID << "\n";
                    mergeTail = firstRoute.edges[firstRoute.edges.size() -1].getDestination();
                    this->routes.erase(thisRoute);
                    thisRoute = routes.begin() + firstRouteIndex;
                }
                    
             }
        }//for
        /*
        for(unsigned k = j+1; k < this->routes.size(); k++){
            Node routeHead = routes[k].edges[0].getOrigin();
            if( mergeTail.nodeID == routeHead.nodeID && (routes[j].vehicleID == routes[k].vehicleID)){
                //write routes[i].edges onto mainRoute.edges
                this->routes[j].edges.insert(this->routes[j].edges.end(), routes[k].edges.begin(), routes[k].edges.end());
                cout << "Moving k onto j. k = " << k << " j = " << j << "\n";
                cout << "J VID: " << this->routes[j].vehicleID << "\n";
                cout << "K VID: " << this->routes[k].vehicleID << "\n";
                auto iterator = this->routes.begin();
                iterator+=k;
                Route iteroute = *iterator;
                cout << "Iterator VID: " << iteroute.vehicleID << "\n";
                this->routes.erase(this->routes.begin()+k);
                mergeFound = true;
                i=0;
                cout << "Breaking loop" << "\n";
                break;
            }
        }
        */
    cout << "Merge routes complete\n";
}

void ScheduleWriter::printRoutes(){
    for(unsigned i=0; i < this->routes.size(); i++){
        printRoute(i);
    }
}

void ScheduleWriter::printRoute(unsigned index){

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
