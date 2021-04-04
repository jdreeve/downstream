#ifndef DOWNSTREAM_SETTINGS_H 
#define DOWNSTREAM_SETTINGS_H 

#include <string>

using namespace std;

class DownstreamSettings{
    public:
        //constructors
        DownstreamSettings(){

        }

        DownstreamSettings(string settingsPath){
            this->settingsPath = settingsPath;
            loadSettings(this->settingsPath);
        }
        
        //public methods
        void loadSettings(string path);
        void printSettings();
        double getEarlyArrivalWindow();
        double getLateArrivalWindow();
        double getServiceDuration();
        double getMaxWait();
        string getVehicleDepotAddress();

    private:
        string vehicleDepotAddress;
        string settingsPath;
        double earlyArrivalWindow;
        double lateArrivalWindow;
        double serviceDuration;
        double maxWait;
        
};

#endif
