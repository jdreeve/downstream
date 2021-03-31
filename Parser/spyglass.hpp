#ifndef SPYGLASS_H
#define SPYGLASS_H
#include <fstream>
#include <string>
#include "json.hpp"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "config.hpp"
using namespace std;
using json = nlohmann::json;

class Spyglass{
    public:
        Spyglass();
        Spyglass(string origin, string destination, DownstreamConfig config);
        void setCourse(string origin, string destination);
        double getTravelDistance();
        double getTravelTime();
        string getOrigin();
        string getDestination();

    private:
        DownstreamConfig config;
        string origin;
        string destination;
        string originPlaceID;
        string destinationPlaceID;
        double travelDistance;
        double travelTime;

        double convertSecondsToMinutes(double seconds);
        void getCourseInfo();
        json performGoogleMapsAPIQuery(string query);
        void performGoogleMapsDistanceMatrixAPIQuery();
        string composeGoogleMapsDistanceMatrixAPIQuery();
        void performGoogleMapsPlacesAPIQuery();
        string composeGoogleMapsPlacesAPIQuery(string location);
        bool findCourseInJSONFile();
        string composeJSONPath();
        bool getCourseFromJSONFile(string jsonPath);
        json loadJSONFromFile(string filePath);
        void writeJSONFile(json j);
        json appendToJSONObject(json original, json appendment);
        bool getCourseFromJSONObject(json j);
        void setOrigin(string origin);
        void setDestination(string destination);
};

#endif
