#include <iostream>
// #include <atomic>
// #include <cctype>
#include <chrono>
// #include <cstring>
// #include <cstdlib>
#include <string>
#include <thread>
// #include <mutex>
// #include <condition_variable>
#include <vector>
#include "mqtt/async_client.h"
#include "mqtt/client.h"
#include "json/json.h"
#include "rover/rover.hpp"

// std::mutex mtx;
// std::condition_variable cv;
bool publishing = true;

const std::string SERVER_ADDRESS	{ "localhost:1883" };
const std::string CLIENT_ID		    { "Rover1" };

void mqttInit(mqtt::client &cli);
void pubMsg(mqtt::client &cli, Rover& rover1);
void subMsg(mqtt::client &cli, Rover& rover1) ;

int main(int argc, char const *argv[])
{   
    mqtt::client cli(SERVER_ADDRESS, CLIENT_ID);
    mqttInit(cli);

    // Init Rover1
    // std::string inputRover1 = R"({
    // "sp": [null, null, null, null],
    // "gp": [null, null, null, null],
    // "gcp": [40, 150, 0],
    // "v": [0, 0, 0],
    // })";
    std::vector<float> robotState{40, 150, 0, 0};
    std::vector<float> obstacle{40, 20, 0, 0};

    Rover rover1 = Rover(robotState, obstacle);

    std::thread t1(pubMsg,std::ref(cli), std::ref(rover1));
    std::thread t2(subMsg,std::ref(cli), std::ref(rover1));

    t1.join();
    t2.join();

    // std::cout<<"Code"<<std::endl;
    return 0;
}


void mqttInit(mqtt::client &cli){
    auto connOpts = mqtt::connect_options_builder()
    // .user_name("user")
    // .password("passwd")
    .keep_alive_interval(std::chrono::seconds(30))
    .automatic_reconnect(std::chrono::seconds(2), std::chrono::seconds(30))
    .clean_session(false)
    .finalize();

    std::cout << "Connecting to the MQTT server..." << std::flush;
    mqtt::connect_response rsp = cli.connect(connOpts);
    std::cout << "OK\n" << std::endl;

    // const std::vector<std::string> TOPICS { "Map/Rover1/Task", "Map/Rover2/State", "Map/Simulasi/#" };
    const std::vector<std::string> TOPICS { "Map/#"};
    const std::vector<int> QOS {0};

    std::cout << "Subscribing to topics..." << std::flush;
    cli.subscribe(TOPICS, QOS);
    std::cout << "OK" << std::endl; 

    // if (!rsp.is_session_present()) {
    //     std::cout << "Subscribing to topics..." << std::flush;
    //     cli.subscribe(TOPICS, QOS);
    //     std::cout << "OK" << std::endl;
    // }
    // else {
    //     std::cout << "Session already present. Skipping subscribe." << std::endl;

    // }

}


void pubMsg(mqtt::client &cli, Rover &rover1){
    while (true){
        if(true){
            std::string msg = rover1.getJSONString();
            auto pubmsg = mqtt::make_message("Map/Rover1/State", msg);
            pubmsg->set_qos(0);
            cli.publish(pubmsg);
            // std::cout << publishing << std::endl;
            // std::this_thread::sleep_for(std::chrono::seconds(2));
            // std::cout << "\nSending message..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

        }
    }
}

void subMsg(mqtt::client &cli, Rover &rover1){
    bool subsc = true;
    while (subsc) {
        auto msg = cli.consume_message();
        if (msg) {
            // std::cout << msg->get_topic() << ": " << msg->to_string() << std::endl;
            // if (msg->get_topic() == "Map/Simulasi/Publish" && msg->to_string() == "true") {
            //     std::cout << "Start Publish...." << std::endl;
            //     publishing = true;
            // }
            // if (msg->get_topic() == "Map/Simulasi/Publish" && msg->to_string() == "false") {
            //     std::cout << "End Publish...." << std::endl;
            //     publishing = false;
            // }
            if (msg->get_topic() == "Map/Simulasi/Command" && msg->to_string() == "display") {
                rover1.display();
            }
            if (msg->get_topic() == "Map/Simulasi/Subscribe" && msg->to_string() == "false") {
                std::cout << "End Subscribe...." << std::endl;
                subsc = false;
            }
            if (msg->get_topic() == "Map/Rover1/Task"){
                std::string msgString = msg->to_string();
                rover1.setTask(msgString); 
                // std::cout << msg->get_topic() << ": " << msg->to_string() << std::endl;
            }
            if (msg->get_topic() == "Map/Rover2/State"){
                std::string msgString = msg->to_string();
                rover1.setObstacle(msgString); 
                // std::cout << msg->get_topic() << ": " << msg->to_string() << std::endl;
            }
            rover1.velocity_obstacle();

        }
        else if (!cli.is_connected()) {
            std::cout << "Lost connection" << std::endl;
            while (!cli.is_connected()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }
            std::cout << "Re-established connection" << std::endl;
        }
        // std::this_thread::sleep_for(std::chrono::seconds(1));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
