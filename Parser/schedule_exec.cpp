#include "parser.hpp"
#include "schedule_writer.hpp"

#define SETTINGS_FILE "./settings.csv"
#define REQUIREMENTS_FILE "./requirements.csv"
#define VEHICLES_FILE "./vehicles.csv"
#define SOLUTION_FILE "./solution.sol"
#define SCHEDULE_FILE "./schedule.csv"

int main(){
    Parser parser = Parser(SETTINGS_FILE, REQUIREMENTS_FILE, VEHICLES_FILE);
    ScheduleWriter writer = ScheduleWriter(parser.getParsedNodes(), parser.getParsedVehicles(), SOLUTION_FILE);
    writer.buildRoutes();
    writer.mergeRoutes();
    writer.writeScheduleFile(SCHEDULE_FILE);
}
