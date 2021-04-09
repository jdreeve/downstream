#include <iostream>
#include "parser.hpp"

int main() {
    cout << "Starting up\n";
    DownstreamConfig config(".config.csv");
    cout << "Config created\n";
    Parser parser(config);
    cout << "Parser created\n";
    parser.writeLPFile(config.lpFilePath);
    parser.printNodes();
    cout << "LP file written\n";
}
