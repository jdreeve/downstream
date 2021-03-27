#include "spyglass.hpp"
#include <filesystem>
#include <iostream>
#define SPYGLASS_DATA_DIR  "./spyglass_data/"
#define API_KEY "AIzaSyAI0KIG3_m5GB_5rUlUiQJfwQzkEulnueM"
using json = nlohmann::json;

//Constructors
Spyglass::Spyglass(){
    this->origin = "";
    this->destination = "";
    this->originPlaceID = "";
    this->destinationPlaceID = "";
    this->travelDistance = -1.0;
    this->travelTime = -1.0;
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
    double travelTimeMinutes = convertSecondsToMinutes(this->travelTime);
    return travelTimeMinutes;
}

double Spyglass::convertSecondsToMinutes(double seconds){
    double minutes;

    minutes = seconds/60;

    return minutes;
}

string Spyglass::getOrigin(){
    return this->origin;
}

string Spyglass::getDestination(){
    return this->destination;
}

//Private methods

void Spyglass::getCourseInfo(){

    if(this->originPlaceID == ""){
    performGoogleMapsPlacesAPIQuery();
    }
    bool courseFoundInJSONFile = findCourseInJSONFile();
    if(!courseFoundInJSONFile){
        performGoogleMapsDistanceMatrixAPIQuery();
    }
}

void Spyglass::performGoogleMapsPlacesAPIQuery(){
    string originQuery = composeGoogleMapsPlacesAPIQuery(this->origin);
 
    json jOrigin = performGoogleMapsAPIQuery(originQuery);

    if(jOrigin["status"] != "OK"){
        cout << "ERROR: query for origin " << this->origin << " returned status " << jOrigin["status"] << "\n";
        return;
    }
    if(jOrigin["candidates"].size() > 1){
        cout << "WARNING: multiple candidate locations found for address " << this->origin << " -- proceeding with first candidate " << jOrigin["candidates"][0] << "\n";
    }

    this->originPlaceID = jOrigin["candidates"][0]["place_id"].get<std::string>();

    string destinationQuery = composeGoogleMapsPlacesAPIQuery(this->destination);
    json jDestination = performGoogleMapsAPIQuery(destinationQuery);

    if(jDestination["status"] != "OK"){
        cout << "ERROR: query for destination " << this->destination << " returned status " << jDestination["status"] << "\n";
        return;
    }
    if(jDestination["candidates"].size() > 1){
        cout << "WARNING: multiple candidate locations found for address " << this->destination << " -- proceeding with first candidate " << jDestination["candidates"][0] << "\n";
    }
    this->destinationPlaceID = jDestination["candidates"][0]["place_id"].get<std::string>();
}

string Spyglass::composeGoogleMapsPlacesAPIQuery(string location){
    string query;

    query = "maps.googleapis.com/maps/api/place/findplacefromtext/json?input=";

    query += location;

    query += "&inputtype=textquery";

    query += "&key=";

    query += API_KEY;

    return query;
}

json Spyglass::performGoogleMapsAPIQuery(string query){
    const char* queryConst = query.c_str();
    httplib::Client client("https://maps.googleapis.com");
    json j;

    auto response = client.Get(queryConst);
    j = json::parse(response->body);
    return j;
}

void Spyglass::performGoogleMapsDistanceMatrixAPIQuery(){
    string query = composeGoogleMapsDistanceMatrixAPIQuery();

    json j = performGoogleMapsAPIQuery(query);

    if(j["status"] == "OK"){
    json original = loadJSONFromFile(composeJSONPath());
    j = appendToJSONObject(original, j);
    writeJSONFile(j);
    }
    //read JSON from file
    //append j to JSON from file
    //write JSON to file
}

void Spyglass::writeJSONFile(json j){
    string filePath = composeJSONPath();
    ofstream file(filePath);

    file << j;

    file.close();
}

string Spyglass::composeGoogleMapsDistanceMatrixAPIQuery(){
    string query;
    string origins = "origins=";
    string destinations = "destinations=";
    string apiKey = "key=";
    apiKey += API_KEY;
    string parameters;

    origins += "place_id:";
    origins += this->originPlaceID;
    destinations += "place_id:";
    destinations += this->destinationPlaceID;
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
    query = "/maps/api/distancematrix/json?" + parameters;
    return query;
}

string formatForGoogleAPIQuery(string input){
    string formattedInput = input;

    for(unsigned i=0; i<formattedInput.size(); i++){
        if(' ' == formattedInput[i]){
            formattedInput[i] = '+';
        }
    }

    return formattedInput;
}

bool Spyglass::findCourseInJSONFile(){
    bool courseFound = false;

    string jsonPath = composeJSONPath();
    courseFound = getCourseFromJSONFile(jsonPath);
    return courseFound;
}

string Spyglass::composeJSONPath(){
    if (this->originPlaceID == ""){
        cout << "ERROR: composeJSONPath() reports origin place ID not set.\n";
    }
    string jsonPath = "";

    jsonPath = SPYGLASS_DATA_DIR;
    jsonPath += this->originPlaceID;
    jsonPath += ".json";
    
    return jsonPath;
}

bool Spyglass::getCourseFromJSONFile(string jsonPath){
    bool courseFound = false;
    json j;
    ifstream jsonFile;

    jsonFile.open(jsonPath);
    if(jsonFile.fail()){
        performGoogleMapsDistanceMatrixAPIQuery();
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
        performGoogleMapsDistanceMatrixAPIQuery();
        jsonFile.open(jsonPath);
        jsonFile >> j;
        courseFound = getCourseFromJSONObject(j);
    }
    if(!courseFound){
        cout << "Error: course not found in origin file\n";
    }
    return courseFound;
}

json Spyglass::loadJSONFromFile(string filePath){
    json j;
    ifstream jsonFile;
    jsonFile.open(filePath);
    if(jsonFile.fail()){
        return j;
    }

    jsonFile >> j;
    jsonFile.close();

    return j;
}

json Spyglass::appendToJSONObject(json original, json appendment){

    original["origin_address"] = this->origin;

    if(appendment["status"] == "OK"){
        original["destination_addresses"].push_back(appendment["destination_addresses"][0]);
        original["rows"].push_back(appendment["rows"][0]);
        original["destination_place_ids"].push_back(this->destinationPlaceID);
    }

    return original;
}

bool Spyglass::getCourseFromJSONObject(json j){
    bool courseFound = false;
    unsigned destinationIndex;

    if(j == NULL){
        return false;
    }

    for(destinationIndex=0; destinationIndex< j["destination_place_ids"].size(); destinationIndex++){
        if(j["destination_place_ids"][destinationIndex] == this->destinationPlaceID){
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


