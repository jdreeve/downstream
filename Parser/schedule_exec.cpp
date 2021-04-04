#include <iostream>
#include "parser.hpp"
#include "schedule_writer.hpp"

int main(){
    DownstreamConfig config(".config.csv");
    cout << "Config created\n";
    Parser parser = Parser(config);
    cout << "Parser created\n";
    ScheduleWriter writer = ScheduleWriter(parser.getParsedNodes(), parser.getParsedVehicles(), config.solutionFilePath);
    cout << "ScheduleWriter created\n";
    writer.buildRoutes();
    cout << "buildRoutes complete\n";
    writer.mergeRoutes();
    cout << "mergeRoutes complete\n";
    writer.writeScheduleFile(config.scheduleFilePath);
}
