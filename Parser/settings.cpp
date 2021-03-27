#include <iostream>
#include <fstream>
#include "settings.hpp"
#include "csv.hpp"

using namespace std;

void DownstreamSettings::loadSettings(string path){
    io::CSVReader<2> settingsCSVReader(path);
    string variable;
    double value;

    while(settingsCSVReader.read_row(variable, value)){
        if(variable == "Early Arrival Window"){
            this->earlyArrivalWindow = value;
        }
        if(variable == "Late Arrival Window"){
            this->lateArrivalWindow = value;
        }
        if(variable == "Service Duration"){
            this->serviceDuration = value;
        }
        if(variable == "Max Wait"){
            this->maxWait = value;
        }
    }
}

void DownstreamSettings::printSettings(){
    cout << "Settings path: " << this->settingsPath << "\n";
    cout << "Early arrival window: " << this->earlyArrivalWindow << "\n";
    cout << "Late arrival window: " <<this->lateArrivalWindow << "\n";
    cout << "Service duration: " << this->serviceDuration << "\n";
    cout << "Max wait time: " << this->maxWait << "\n";
}

double DownstreamSettings::getEarlyArrivalWindow(){
    return this->earlyArrivalWindow;
}

double DownstreamSettings::getLateArrivalWindow(){
    return this->lateArrivalWindow;
}

double DownstreamSettings::getServiceDuration(){
    return this->serviceDuration;
}

double DownstreamSettings::getMaxWait(){
    return this->maxWait;
}
