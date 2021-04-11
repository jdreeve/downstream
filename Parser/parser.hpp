#ifndef PARSER_H
#define PARSER_H
#include <vector>
#include <string>
#include <iostream>
#include "config.hpp"
#include "settings.hpp"
#include "node.hpp"
#include "vehicle.hpp"

using namespace std;

class Parser{
    public:
        Parser(DownstreamConfig config){
            cout << "Initializing parser\n";
            this->config = config;
            this->settings = DownstreamSettings(this->config.solverSettingsFilePath);
            cout << "Parsing requirements\n";
            parseRequirements(this->config.requirementsFilePath);
            cout << "Loading vehicles\n";
            getVehicles(this->config.vehiclesFilePath);
            cout << "Loading vehicles complete.\n";
        }
		void parseRequirements(string requirementsPath);
		int convert24HourTimeToMinutes(int timeIn24Hr);
        int addTimeOffset(int timeIn24Hr);
		void getVehicles(string vehiclesPath);
		void writeLPFile(string filePath);
		string getObjective();
        string getBinaryVars();
        vector<Vehicle> getParsedVehicles();
        vector<Node> getParsedNodes();
		void writeObjective(string filePath);
		void writeConstraints(string filePath);
		void writeBinarySection(string filePath);
		void writeTerminal(string filePath);
		void printNodes();
		void printVehicles();
		string cordeauConstraint2();
		string cordeauConstraint3();
		string cordeauConstraint4();
		string cordeauConstraint5();
		string cordeauConstraint6();
		string cordeauConstraint7();
		string cordeauConstraint8();
		string cordeauConstraint11();
		string cordeauConstraint13();
		int getTransitTime(Node i, Node j);
		
		string buildConstantTerm(int constant, string operation);
		string buildThreeIndexedTerm(int coefficient, int origin, int destination, int vehicle, string operation);
		string buildStartServiceTerm(int coefficient, int nodeIndex, int vehicle, string operation);
		string buildTwoIndexedTerm(int coefficient, string variable, string index1Variable, int index1Value, string index2Variable, int index2Value);
		void writeLengthCappedString(string &constraint, string term);
		string buildIndent(int depth);

        DownstreamConfig config;
        DownstreamSettings settings;
        vector<Node> nodes;
        vector<Vehicle> vehicles;
};

#endif
