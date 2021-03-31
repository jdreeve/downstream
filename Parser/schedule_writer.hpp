#ifndef SCHEDULE_WRITER_H
#define SCHEDULE_WRITER_H

#include "settings.hpp"
#include "edge.hpp"
#include "parser.hpp"

#define SOL_FILE = "./solution.sol"

using namespace std;

class Route{
    public:
        vector<Edge> edges;
        int vehicleID;

        Route(){
        }
};

class ScheduleWriter{
    public:
        //constructor

        ScheduleWriter(){
        }

        ScheduleWriter(vector<Node> nodes, vector<Vehicle> vehicles, string solutionPath){
            setNodes(nodes);
            setVehicles(vehicles);
            setSolutionPath(solutionPath);
        }
        
        void setNodes(vector<Node> nodes){
            this->nodes = nodes;
        }

        void setVehicles(vector<Vehicle> vehicles){
            this->vehicles = vehicles;
        }

        void setSolutionPath(string solutionPath){
            this->solutionPath = solutionPath;
        }

        void writeSchedule();
        void buildRoutes();
        void mergeRoutes();

        void printRoutes();
        void printRoute(unsigned index);
        void writeScheduleFile(string filePath);

    private:
        string solutionPath;
        vector<Node> nodes;
        vector<Vehicle> vehicles;
        vector<Route> routes;

        void processSolutionLine(string line);
        void processSolutionEdge(string token);
        void processSolutionTime(string token, double value);
};

#endif
