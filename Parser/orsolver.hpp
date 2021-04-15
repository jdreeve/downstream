#ifndef ORSOLVER_H
#define ORSOLVER_H

#include "config.hpp"
#include "settings.hpp"
#include "node.hpp"
#include "vehicle.hpp"
#include "parser.hpp"
#include "spyglass.hpp"
#include "ortools/linear_solver/linear_solver.h"

#define DEBUG 0

using namespace operations_research;


//ORSolver solves problems generated by Parser object. Note that Cordeau constraints 9 (max ride time per user) and 10 (max tour time per vehicle) are not currently implemented as they are not required by my client.
class ORSolver{
    public:
        ORSolver(Parser parser){
            this->solver = MPSolver::CreateSolver("SCIP");
            this->vehicles = parser.getParsedVehicles();
            this->nodes = parser.getParsedNodes();
            this->config = parser.config;
            this->n = (this->nodes.size()-2)/2;

            this->infinity = solver->infinity();

            //allocate decision variable vector
            x.resize(nodes.size());
            for(unsigned i=0; i < this->nodes.size(); i++){
                x[i].resize(nodes.size());

                for(unsigned j=0; j < this->nodes.size(); j++){
                    x[i][j].resize(vehicles.size());
                }
            }

            //allocate service start times vector and load vector
            b.resize(nodes.size());
            q.resize(nodes.size());
            for(unsigned i=0; i < this->nodes.size(); i++){
                b[i].resize(vehicles.size());
                q[i].resize(vehicles.size());
            }
        }
    
        void generateModel(){
            populateDecisionVariables();
            populateStartTimeVariables();
            populateLoadVariables();
            imposeCordeauConstraints();
            generateObjectiveFunction();
        }

        void solveModel(){
            this->resultStatus = this->solver->Solve();
        }

        void displaySolution(){
            if(this->resultStatus != MPSolver::OPTIMAL){
                LOG(FATAL) << "No optimal solution found.";
            }
            LOG(INFO) << "Solution:";
            LOG(INFO) << "Optimal objective value = " << this->objective->Value();

            if(DEBUG){
                cout << "n = " << this->n << "\n";
                cout << "nodes.size() = " << this->nodes.size() << "\n";
                cout << "x.size() = " << x.size() << "\n";
                cout << "x[0].size() = " << x[0].size() << "\n";
                cout << "x[0][0].size() = " << x[0][0].size() << "\n";
            }
            for(unsigned i=0; i < x.size(); i++){
                for(unsigned j=0; j < x[i].size(); j++){
                    for(unsigned k = 0; k < x[i][j].size(); k++){
                        if( x[i][j][k] != NULL){
                            int solutionValue = x[i][j][k]->solution_value();
                            if(solutionValue != 0){
                                LOG(INFO) << "x[" << i << "][" << j << "][" << k << "] = " << solutionValue;
                                LOG(INFO) << "\tb[" << i << "][" << k << "] = " << b[i][k]->solution_value();
                            }
                        }
                    }
                }
            }
        }

        void saveSolution();

    private:
        DownstreamConfig config;
        MPSolver* solver;
        MPObjective* objective;
        MPSolver::ResultStatus resultStatus;
        vector<Node> nodes;
        vector<Vehicle> vehicles;
        double infinity;
        //number of pickup nodes
        unsigned n;
        //3D vector for decision variables - if x[i][j][k] = 1, vehicle k serves the trip from i to j
        std::vector<std::vector<std::vector<MPVariable*> > > x;
        //2D vector for start times - b[i][k] = the time vehicle k starts service at node i
        std::vector<std::vector<MPVariable*> > b;
        //2D vector for vehicle load - q[i][k] = load of vehicle k after visiting node i
        std::vector<std::vector<MPVariable*> > q;

        void populateDecisionVariables(){
            imposeCordeauConstraint14();
        }

        void populateStartTimeVariables(){
            for(unsigned i=0; i < this->nodes.size(); i++){
                for(unsigned k = 0; k < this->vehicles.size(); k++){
                    b[i][k] = this->solver->MakeIntVar(-1440, 2880, "");
                }
            }
        }

        void populateLoadVariables(){
            for(unsigned k = 0; k < this->vehicles.size(); k++){
                int maxCapacity = this->vehicles[k].capacity;
                for(unsigned i = 0; i < this->nodes.size(); i++){
                    q[i][k] = this->solver->MakeIntVar(0, maxCapacity, "");
                }
            }
        }

        void imposeCordeauConstraints(){
            //imposeCordeauConstraint14(); //handled by populateDecisionVariables()
            imposeCordeauConstraint2();
            imposeCordeauConstraint3();
            imposeCordeauConstraint4();
            imposeCordeauConstraint5();
            imposeCordeauConstraint6();
            imposeCordeauConstraint7();
            imposeCordeauConstraint8();
            imposeCordeauConstraint11();
            imposeCordeauConstraint13();

        }

        //Cordeau constraint 2: visit every pickup node
        void imposeCordeauConstraint2(){
            for(unsigned i=1; i <= n; i++){
                MPConstraint* cordeauConstraint2 = solver->MakeRowConstraint(1, 1, "cordeauConstraint2");
                for(unsigned k = 0; k < this->vehicles.size(); k++){
                    for(unsigned j=0; j < this->nodes.size(); j++){
                        if(i!=j){
                            cordeauConstraint2->SetCoefficient(x[i][j][k], 1);
                        }
                    }
                }
            }
            if(DEBUG){
                cout << "DEBUG: imposeCordeauConstraint2() reporting:\n";
                cout << "\t" << "Constraints: " << this->solver->NumConstraints() << "\n";
                cout << "\t" << "Reference: should be " << this->n << "\n\n";
            }
        }

        //Cordeau constraint 3: same vehicle must visit pickup and dropdown nodes
        void imposeCordeauConstraint3(){
            for(unsigned i = 1; i <= n; i++){
                for(unsigned k = 0; k < this->vehicles.size(); k++){
                    MPConstraint* cordeauConstraint3 = solver->MakeRowConstraint(0, 0, "");
                    for(unsigned j = 0; j < this->nodes.size(); j++){
                        if(i!=j){
                            cordeauConstraint3->SetCoefficient(x[i][j][k], 1);
                        }
                    }
                    for(unsigned j = 0; j < this->nodes.size(); j++){
                        if(i!=j){
                            if (n+i != j){
                                cordeauConstraint3->SetCoefficient(x[n+i][j][k], -1);
                            }
                        }
                    }
                }
            }
            if(DEBUG){
                cout << "DEBUG: Cordeau constraint 3 reporting.\n\n";
            }
        }

        //Cordeau constraint 4: Every vehicle's route begins at origin depot
        void imposeCordeauConstraint4(){
            for(unsigned k = 0; k < this->vehicles.size(); k++){
                MPConstraint* cordeauConstraint4 = solver->MakeRowConstraint(1, 1, "cordeauConstraint4");
                for(unsigned j = 1; j < this->nodes.size(); j++){
                        cordeauConstraint4->SetCoefficient(x[0][j][k], 1);
                }
            }
            if(DEBUG){
                cout << "DEBUG: Cordeau constraint 4 reporting.\n\n";
            }
        }

        //Cordeau constraint 5: every pickup and dropoff node (nodes 1-2n) must have the same number of inbound and outbound edges. Nodes must be visited by the same vehicle.
        void imposeCordeauConstraint5(){
            for(unsigned i = 1; i < 2*n; i++){
                for(unsigned k = 0; k < this->vehicles.size(); k++){
                    MPConstraint* cordeauConstraint5 = solver->MakeRowConstraint(0, 0, "");
                    for(unsigned j = 0; j < this->nodes.size(); j++){
                        if(i!=j){
                            cordeauConstraint5->SetCoefficient(x[j][i][k], 1);
                            cordeauConstraint5->SetCoefficient(x[i][j][k], -1);
                        }
                    }
                }
            }
            if(DEBUG){
                cout << "DEBUG: Cordeau constraint 5 reporting.\n\n";
            }
        }

        //Cordeau constraint 6: all vehicles end their routes at the destination depot
        void imposeCordeauConstraint6(){
            for(unsigned k = 0; k < this->vehicles.size(); k++){
                MPConstraint* cordeauConstraint6 = solver->MakeRowConstraint(1, 1, "");
                for(unsigned i = 0; i < (this->nodes.size() - 1); i++){
                    cordeauConstraint6->SetCoefficient(x[i][(2*n)+1][k], 1);
                }
            }
            if(DEBUG){
                cout << "DEBUG: Cordeau constraint 6 reporting.\n\n";
            }
        }

        //Cordeau constraint 7: service at node j (destination) has to start after service at node i (corresponding origin) is finished and after the vehicle has driven from i to j. Implemented as linearized Cordeau constraint 15.
        // d[i] = service duration at i, t[i][j] = transit time from node i to node j
        // b[i][k] - b[j][k] + (M[i][j][k] * x[i][j][k]) <= M[i][j][k] - d[i] - t[i][j]
        void imposeCordeauConstraint7(){
            for(unsigned i=0; i < this->nodes.size(); i++){
                for(unsigned j = 0; j < this->nodes.size(); j++){
                    if(i!=j){
                        int transitTime = getTransitTime(nodes[i], nodes[j]);
                        for(unsigned k = 0; k < this->vehicles.size(); k++){
                            int m = nodes[i].latestServiceTime + nodes[i].serviceDuration + transitTime - nodes[j].earliestServiceTime;
                            if(m < 0){
                                m = 0;
                            }
                            int upperBound = m - nodes[i].serviceDuration - transitTime;

                            MPConstraint* cordeauConstraint7 = solver->MakeRowConstraint(-infinity, upperBound, "");
                            cordeauConstraint7->SetCoefficient(b[i][k], 1);
                            cordeauConstraint7->SetCoefficient(b[j][k], -1);
                            cordeauConstraint7->SetCoefficient(x[i][j][k], m);

                        }
                    }
                }
            }
            if(DEBUG){
                cout << "DEBUG: Cordeau constraint 7 reporting.\n\n";
            }
        }

        //Cordeau constraint 8: load on vehicle k at node i + load of node j <= capacity of vehicle k. Implemented as linearized Cordeau constraint 16.
        // q[i][k] - q[j][k] + W[i][k] * x[i][j][k] <= W[i][k] - nodes[j].load
        void imposeCordeauConstraint8(){
            double w;
            for(unsigned i = 0; i < this->nodes.size(); i++){
                for(unsigned j = 0; j < this->nodes.size(); j++){
                    if(i != j){
                        for(unsigned k = 0; k < this->vehicles.size(); k++){
                            w = vehicles[k].capacity + nodes[i].load;
                            if(w < vehicles[k].capacity){
                                w = vehicles[k].capacity;
                            }

                            int upperBound = w - nodes[j].load;

                            MPConstraint* cordeauConstraint8 = solver->MakeRowConstraint(-infinity, upperBound, "");
                            cordeauConstraint8->SetCoefficient(q[i][k], 1);
                            cordeauConstraint8->SetCoefficient(q[j][k], -1);
                            cordeauConstraint8->SetCoefficient(x[i][j][k], w);
                        }
                    }
                }
            }
            if(DEBUG){
                cout << "DEBUG: Cordeau constraint 8 reporting.\n\n";
            }
        }

        //Cordeau constraint 11: nodes must be visited within their service time window.
        void imposeCordeauConstraint11(){
            for(unsigned i = 0; i < this->nodes.size(); i++){
                for(unsigned k = 0; k < this->vehicles.size(); k++){
                    int lowerBound = nodes[i].earliestServiceTime;
                    int upperBound = nodes[i].latestServiceTime;
                    MPConstraint* cordeauConstraint11 = solver->MakeRowConstraint(lowerBound, upperBound, "");
                    cordeauConstraint11->SetCoefficient(b[i][k], 1);
                }
            }
            if(DEBUG){
                cout << "DEBUG: Cordeau constraint 11 reporting.\n\n";
            }
        }

        //Cordeau constraint 13: vehicle load must be between 0 and vehicle capacity.
        // 0 <= q[i][k] <= vehicles[k].capacity
        void imposeCordeauConstraint13(){
            for(unsigned i = 0; i < this->nodes.size(); i++){
                for(unsigned k = 0; k < this->vehicles.size(); k++){
                    int upperBound = vehicles[k].capacity;
                    MPConstraint* cordeauConstraint13 = solver->MakeRowConstraint(0, upperBound, "");
                    cordeauConstraint13->SetCoefficient(q[i][k], 1);
                }
            }
            if(DEBUG){
                cout << "DEBUG: Cordeau constraint 13 reporting.\n\n";
            }
        }


        //Cordeau constraint 14: variables in x are binary
        void imposeCordeauConstraint14(){
            for(unsigned i=0; i < this->nodes.size(); i++){
                for(unsigned j= 0; j < this->nodes.size(); j++){
                    if(i!=j){
                        for(unsigned k=0; k < this->vehicles.size(); k++){
                            x[i][j][k] = this->solver->MakeIntVar(0, 1, "");
                        }
                    }
                }
            }
            if(DEBUG){
                cout << "DEBUG: Cordeau constraint 14 reporting.\n\n";
            }
        }



        void generateObjectiveFunction(){
            int counter = 0;
            this->objective = solver->MutableObjective();
            for(unsigned i=0; i < this->nodes.size(); i++){
                for(unsigned j=0; j < this->nodes.size(); j++){
                    if(i!=j){
                        unsigned transitTime = getTransitTime(this->nodes[i], this->nodes[j]);
                        for(unsigned k=0; k<vehicles.size(); k++){
                            objective->SetCoefficient(x[i][j][k], transitTime);
                            if(DEBUG){
                                counter++;
                            }
                        }
                    }
                }
            }
            objective->SetMinimization();
            if(DEBUG){
                cout << "DEBUG: generateObjectiveFunction() \n\tObjective function has " << counter << "terms\n";
                cout << "\tReference: should be nodes.size() * (nodes.size()-1) * k = " << this->nodes.size() * (this->nodes.size()-1) * (this->vehicles.size()) << "\n\n";
            }
        }

        int getTransitTime(Node i, Node j){
            /*
            Spyglass spyglass = Spyglass(i.address, j.address, this->config);
            int travelTime = spyglass.getTravelTime();
            return travelTime;
            */
            return 1;
        }

};

#endif
