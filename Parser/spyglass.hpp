#ifndef SPYGLASS_H
#define SPYGLASS_H
#include <fstream>
#include <string>
#include "json.hpp"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
using namespace std;
using json = nlohmann::json;

class Spyglass{
    public:
        Spyglass();
        Spyglass(string origin, string destination);
        void setCourse(string origin, string destination);
        double getTravelDistance();
        double getTravelTime();
        string getOrigin();
        string getDestination();

    private:
        string origin;
        string destination;
        double travelDistance;
        double travelTime;

        void getCourseInfo();
        void performGoogleMapsAPIQuery();
        string composeGoogleMapsAPIQuery();
        bool findCourseInJSONFile();
        string composeJSONPath();
        bool getCourseFromJSONFile(string jsonPath);
        json appendToJSONObject(json original, json appendment);
        bool getCourseFromJSONObject(json j);
        void setOrigin(string origin);
        void setDestination(string destination);
};

#endif
