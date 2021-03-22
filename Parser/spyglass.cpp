#include "spyglass.hpp"
#include <filesystem>
#include <iostream>

#define SPYGLASS_DATA_DIR  "./spyglass_data/"

using json = nlohmann::json;

//Constructors
Spyglass::Spyglass(){
    this->origin = "";
    this->destination = "";
    this->travelDistance = -1;
    this->travelTime = -1;
}

Spyglass::Spyglass(string origin, string destination){
    this->origin = origin;
    this->destination = destination;
    this->travelDistance = -1;
    this->travelTime= -1;
    getCourseInfo();
}

//Public methods

void Spyglass::setCourse(string origin, string destination){
    setOrigin(origin);
    setDestination(destination);
    getCourseInfo();
}

double Spyglass::getTravelDistance(){
    return this->travelDistance;
}

double Spyglass::getTravelTime(){
    return this->travelTime;
}

string Spyglass::getOrigin(){
    return this->origin;
}

string Spyglass::getDestination(){
    return this->destination;
}

//Private methods

void Spyglass::getCourseInfo(){
    bool courseFoundInJSONFile = findCourseInJSONFile();
    if(!courseFoundInJSONFile){
        performGoogleMapsAPIQuery();
    }

}

void Spyglass::performGoogleMapsAPIQuery(){

}

string Spyglass::composeGoogleMapsAPIQuery(){
    string query;
    string origins = "origins=";
    string destinations = "destinations=";
    string apiKey = "key=";
    string parameters;

    origins += this->origin;
    destinations += this->destination;
    //replace all spaces in origin, destination addresses with + signs - API requirement
    for(unsigned i=0; i<origins.size(); i++){
        if(' ' == origins[i]){
            origins[i] = '+';
        }
    }
    for(unsigned i=0; i<destinations.size(); i++){
        if(' ' == destinations[i]){
            destinations[i] = '+';
        }
    }
    parameters = origins + "&" + destinations + "&" + apiKey;
    query = "http://maps.googleapis.com/maps/api/distancematrix/json?" + parameters;
    return query;
}

bool Spyglass::findCourseInJSONFile(){
    bool courseFound = false;

    string jsonPath = composeJSONPath();
    courseFound = getCourseFromJSONFile(jsonPath);

    return courseFound;
}

string Spyglass::composeJSONPath(){
    string jsonPath = "";

    jsonPath = SPYGLASS_DATA_DIR;
    jsonPath += this->origin;
    jsonPath += ".json";
    
    return jsonPath;
}

bool Spyglass::getCourseFromJSONFile(string jsonPath){
    bool courseFound = false;
    json j;
    ifstream jsonFile;

    jsonFile.open(jsonPath);
    if(jsonFile.fail()){
        performGoogleMapsAPIQuery();
        jsonFile.open(jsonPath);
    }
    if(jsonFile.fail()){
        cout << "Error: origin file not available\n";
        return courseFound;
    }

    jsonFile >> j;

    courseFound = getCourseFromJSONObject(j);
    //find index of destination address
    if(courseFound){
        return courseFound;
    }
    else{//if JSON object does not contain desired course
        jsonFile.close();
        performGoogleMapsAPIQuery();
        jsonFile.open(jsonPath);
        jsonFile >> j;
        courseFound = getCourseFromJSONObject(j);
    }
    if(!courseFound){
        cout << "Error: course not found in origin file\n";
    }
    return courseFound;
}

json Spyglass::appendToJSONObject(json original, json appendment){
    original["destination_addresses"].push_back(appendment["destination_addresses"][0]);
    original["rows"].push_back(appendment["rows"][0]);

    /*
    json k;
    cout << "First push\n";
    k.push_back("figgin");
    cout << "Second push\n";
    k.push_back("second figgin");
    cout << "k: " << k << "\n";
    json l;
    l["destination_addresses"] = k;
    cout << "l: " << l << "\n";
*/

    return original;
}

bool Spyglass::getCourseFromJSONObject(json j){
    bool courseFound = false;
    unsigned destinationIndex;

    for(unsigned i=0; i<j["destination_addresses"].size(); i++){
    cout << j["destination_addresses"][i].get<std::string>() << "\n";
    }


    for(destinationIndex=0; destinationIndex< j["destination_addresses"].size(); destinationIndex++){
        if(j["destination_addresses"][destinationIndex] == this->destination){
            courseFound = true;
            break;
        }
    }


    if(courseFound){
        json k = j["rows"][destinationIndex]["elements"][0];
        this->travelDistance = k["distance"]["value"];
        this->travelTime = k["duration"]["value"];
    }
    return courseFound;
}

void Spyglass::setOrigin(string origin){
    this->origin = origin;
}

void Spyglass::setDestination(string destination){
    this->destination = destination;
}


