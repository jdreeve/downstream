#include <iostream>
#include "parser.hpp"
#include "orsolver.hpp"

int main() {
    DownstreamConfig config(".config.csv");
    Parser parser(config);
    parser.splitNodeVector();
    if(parser.nodes.size() > 40){
        parser.printSplitNodeVectors();
        cout << "Vectors split\n";
        for(unsigned nodeVectorIndex = 0; nodeVectorIndex < parser.splitNodeVectors.size(); nodeVectorIndex++){
            ORSolver solver(parser, nodeVectorIndex);
            cout << "Solver generated for node vector " << to_string(nodeVectorIndex) << "\n";
            solver.generateModel();
            cout << "Model generated\n";
            solver.solveModel();
            cout << "Model solved\n";
            solver.generateSchedule();
            cout << "Schedule written";
            solver.displaySolution();
            cout << "\n\n";
        }
    }
    else{
        ORSolver solver(parser);
        solver.generateModel();
        solver.solveModel();
        solver.generateSchedule();
        solver.displaySolution();
    }
}
