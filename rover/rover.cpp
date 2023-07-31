#include <iostream>
#include <vector>
#include "json/json.h"
#include "rover.hpp"


Rover::Rover(const std::string& inputDataRover) {

    std::istringstream jsonStream(inputDataRover);
    bool parsingSuccessful = Json::parseFromStream(builder, jsonStream, &dataRover, &errs);
    if (!parsingSuccessful) {
        std::cout << "Failed to parse JSON: " << errs << std::endl;
    }

    else{
    const Json::Value& spArray = dataRover["sp"];
    for (int i = 0; i < spArray.size(); ++i) {
        robotState.push_back(spArray[i].asFloat());
        sp.push_back(spArray[i].asFloat());
    }

    const Json::Value& gpArray = dataRover["gp"];
    for (int i = 0; i < gpArray.size(); ++i) {
        gp.push_back(gpArray[i].asFloat());
    }

    const Json::Value& gcpArray = dataRover["gcp"];
    for (int i = 0; i < gcpArray.size(); ++i) {
        gcp.push_back(gcpArray[i].asFloat());
    }

    const Json::Value& vArray = dataRover["v"];
    for (int i = 0; i < vArray.size(); ++i) {
        v.push_back(vArray[i].asFloat());
    }
    }

    readyMove = false;

}


void Rover::updateState(std::vector<float> robotState) {
    std::cout << "Updating Rover State..." << std::endl;
    this->robotState = robotState;
    gcp[0] = robotState[0];
    gcp[1] = robotState[1];
    v[0] = robotState[2];
    v[1] = robotState[3];
    // update JSON
    dataRover["gcp"][0] = robotState[0];
    dataRover["gcp"][1] = robotState[1];
    dataRover["v"][0] = robotState[2];
    dataRover["v"][1] = robotState[3];
}


std::vector<float> Rover::getState() {
    std::cout << " Robot State  = ";
    printVector(robotState);
    return robotState;
}


void Rover::setReadyMove(bool input) {
    std::cout << "Set Ready Move" << std::endl;
    readyMove = input;
}


bool Rover::getReadyMove() {
    std::cout << " readyMove = " << readyMove << std::endl;
    return readyMove;
}


void Rover::setTask(std::string inputTask) {
    Json::CharReaderBuilder builderTask;
    Json::Value task;
    Json::String errsTask;
    std::istringstream jsonStream(inputTask);
    bool parsingSuccessful = Json::parseFromStream(builder, jsonStream, &task, &errsTask);
    if (!parsingSuccessful) {
        std::cout << "Failed to parse JSON: " << errs << std::endl;

    }
    else{
    std::cout << "Setting Rover Task..." << std::endl;
    const Json::Value& spArray = task["sp"];
    for (int i = 0; i < spArray.size(); ++i) {
        // Update values of vector sp and JSON data Rover
        sp[i] = spArray[i].asFloat();
        dataRover["sp"][i] = spArray[i].asFloat();
        // std::cout<<spArray[i]<<std::endl;
    }
    const Json::Value& gpArray = task["gp"];
    for (int i = 0; i < gpArray.size(); ++i) {
        // Update values of vector gp and JSON data Rover
        gp[i] = gpArray[i].asFloat();
        dataRover["gp"][i] = gpArray[i].asFloat();
        // std::cout<<gpArray[i]<<std::endl;
    }
    }
}


std::vector<float> Rover::getTask() {
    std::cout << " gp = ";
    printVector(gp);
    return gp;
}


// std::vector<std::vector<float>> Rover::getMessage() {
//     return dataRover;
// }

// void Rover::updateStateHistory(std::vector<float> input) {
//     for (int i = 0; i < 4; ++i) {
//         stateHistory[i][step] = input[i];
//     }
//     step++;
// }

// std::vector<std::vector<float>> Rover::getStateHistory() {
//     return stateHistory;
// }

std::string Rover::getJSONString() {
    // Serialize the JSON object to a string
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "\t";
    std::string jsonString = Json::writeString(writer, dataRover);
    std::cout << " JSON String = " << jsonString << std::endl;
    return jsonString;
}

// Print Vector
void Rover::printVector(std::vector<float> vec){
        for (int i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << " ";
    }
    std::cout << std::endl;
}


void Rover::display(){
    std::cout << "Display private atributes" << std::endl;
    std::cout << " Robot State  = ";
    printVector(robotState);
    std::cout << " Ready Move   = " <<readyMove << std::endl;
    std::cout << " Start Point  = ";
    printVector(sp);
    std::cout << " Goal Point   = ";
    printVector(gp);
    std::cout << " gcp          = ";
    printVector(gcp);
    std::cout << " v            = ";
    printVector(v);
    std::cout << " Data Rover   = " << dataRover << std::endl;
    std::cout << std::endl;


}