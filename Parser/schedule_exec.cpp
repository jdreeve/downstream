#include "parser.hpp"
#include "schedule_writer.hpp"

int main(){
    DownstreamConfig config(".config.csv");
    Parser parser = Parser(config);
    ScheduleWriter writer = ScheduleWriter(parser.getParsedNodes(), parser.getParsedVehicles(), config.solutionFilePath);
    writer.buildRoutes();
    writer.mergeRoutes();
    writer.writeScheduleFile(config.scheduleFilePath);
}
