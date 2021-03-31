#include "parser.hpp"

#define SETTINGS_FILE "./settings.csv"
#define REQUIREMENTS_FILE "./requirements.csv"
#define VEHICLES_FILE "./vehicles.csv"
#define LP_FILE "requirements.lp"

int main() {
    Parser parser(SETTINGS_FILE, REQUIREMENTS_FILE, VEHICLES_FILE);
    parser.writeLPFile(LP_FILE);
}
