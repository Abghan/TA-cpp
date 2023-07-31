#pragma once
#include <vector>
// #include <string>
#include <json/json.h>

class Rover {
private:
    std::vector<float> robotState;
    bool readyMove;
    // int step;
    // std::vector<std::vector<float>> stateHistory;

    std::vector<float> sp;
    std::vector<float> gp;
    std::vector<float> gcp;
    std::vector<float> v;

    //make JSON from string
    Json::CharReaderBuilder builder;
    Json::Value dataRover;
    Json::String errs;

public:
    Rover(const std::string& inputDataRover); 

    void updateState(std::vector<float> robotState);

    std::vector<float> getState();

    void setReadyMove(bool input);

    bool getReadyMove();

    void setTask(std::string inputTask); 

    std::vector<float> getTask();

    // void updateStateHistory(std::vector<float> input);

    // std::vector<std::vector<float>> getStateHistory();

    std::string getJSONString();
    
    // Print Vector
    void printVector(std::vector<float> vec);

    void display();
};