#include "spyglass.hpp"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int assertSpyglassParameters(Spyglass testSpyglass, string origin, string destination, double travelDistance, double travelTime){
    int errors = 0;
    if(testSpyglass.getOrigin() != origin){
        errors++;
        cout << "Origin error: " << testSpyglass.getOrigin() << " -- should be " << origin << "\n";
    }

    if(testSpyglass.getDestination() != destination){
        errors++;
        cout << "destination error: " << testSpyglass.getDestination() << " -- should be " << destination << "\n";
    }
    if(testSpyglass.getTravelDistance() != travelDistance){
        errors++;
        cout << "Travel Distance Error: " << testSpyglass.getTravelDistance() << " -- should be " << travelDistance << "\n";
    }

    if(testSpyglass.getTravelTime() != travelTime/60){
        errors++;
        cout << "Travel Time Error: " << testSpyglass.getTravelTime() << " -- should be " << travelTime << "\n";
    }

    return errors;
}

int testSpyglassNullConstructor(){
    int errors = 0;

    Spyglass testSpyglass = Spyglass();

    errors += assertSpyglassParameters(testSpyglass, "", "", -1, -1);

    cout << "Null constructor test complete. Errors: " << errors << "\n";

    return errors;
}

int testSpyglassParameterizedConstructorInvalidDestination(){
    int errors = 0;
    string origin = "testOrigin";
    string destination = "invalidDestination";

    Spyglass testSpyglass = Spyglass(origin, destination);

    errors += assertSpyglassParameters(testSpyglass, origin, destination, -1, -1);

    cout << "Parameterized constructor (invalid destination) test complete. Errors: " << errors << "\n";
    return errors;
}

int testSpyglassParameterizedConstructorValidDestination(){
    int errors = 0;
    string origin = "testOrigin";
    string destination = "testDestination";

    cout << "Testing parameterized constructor.\n";
    Spyglass testSpyglass = Spyglass(origin, destination);

    errors += assertSpyglassParameters(testSpyglass, origin, destination, -1, -1);

    cout << "Parameterized constructor test complete. Errors: " << errors << "\n";
    return errors;
}

int testSpyglassSetCourse(){
    int errors = 0;
    string origin = "testOrigin";
    string destination = "testDestination";

    cout << "Testing setCourse method.\n";
    Spyglass testSpyglass = Spyglass();
    testSpyglass.setCourse(origin, destination);
    errors += assertSpyglassParameters(testSpyglass, origin, destination, 2468, 1234);

    
    cout << "setCourse() method test complete. Errors: " << errors << "\n";
    return errors;
}

int testSpyglassGoogleMapsAPI(){
    int errors = 0;

    cout << "Testing Google Maps API.\n";
    Spyglass testSpyglass = Spyglass();
    string origin ="100 Ivy Hill Rd Weaverville NC";
    string destination = "25 Northridge Commons Pkwy Weaverville NC";
    testSpyglass.setCourse(origin, destination);

    cout << "Travel distance: " << testSpyglass.getTravelDistance() << "\n";
    cout << "Approximate travel time: " << testSpyglass.getTravelTime() << "\n";
    cout << "Spyglass Google Maps API test complete.\n";
    return errors;
}

int main(){
    int totalErrors = 0;

    totalErrors += testSpyglassNullConstructor();

    totalErrors += testSpyglassParameterizedConstructorValidDestination();

    totalErrors += testSpyglassGoogleMapsAPI();

    cout << "Spyglass testing complete. Total errors: " << totalErrors << "\n";
    return 0;
    
}


