#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#include "csv.hpp"

using namespace std;

class DownstreamConfig{
    public:
        string configPath;
        string solverSettingsFilePath;
        string requirementsFilePath;
        string vehiclesFilePath;
        string lpFilePath;
        string spyglassDataPath;
        string spyglassAPIKey;
        string solutionFilePath;
        string scheduleFilePath;

        DownstreamConfig(){

        }

        DownstreamConfig(string configPath){
            this->configPath = configPath;
            loadConfig(this->configPath);
        }

        void loadConfig(string configPath){
            io::CSVReader<2> configCSVReader(configPath);
            string variable;
            string value;
            while(configCSVReader.read_row(variable, value)){

                if(variable == "solverSettingsFilePath"){
                    this->solverSettingsFilePath = value;
                }
                if(variable == "requirementsFilePath"){
                    this->requirementsFilePath = value;
                }
                if(variable == "vehiclesFilePath"){
                    this->vehiclesFilePath = value;
                }
                if(variable == "lpFilePath"){
                    this->lpFilePath = value;
                }
                if(variable == "spyglassDataPath"){
                    this->spyglassDataPath = value;
                }
                if(variable == "spyglassAPIKey"){
                    this->spyglassAPIKey = value;
                }
                if(variable == "solutionFilePath"){
                    this->solutionFilePath = value;
                }
                if(variable == "scheduleFilePath"){
                    this->scheduleFilePath = value;
                }
            }
            
        }

};
#endif
