#include <iostream>
#include "parser.hpp"
#include "orsolver.hpp"

int main() {
    cout << "Starting up\n";
    DownstreamConfig config(".config.csv");
    cout << "Config created\n";
    Parser parser(config);
    cout << "Parser created\n";
    parser.printNodes();
    ORSolver solver(parser);
    solver.generateModel();
    solver.solveModel();
    solver.displaySolution();
}
