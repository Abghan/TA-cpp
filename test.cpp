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
// bool publishing = true;
const std::string SERVER_ADDRESS	{ "tcp://localhost:1883" };
const std::string CLIENT_ID		    { "Rover1" };

void mqttInit();
void pubMsg();
void subMsg();

mqtt::client cli(SERVER_ADDRESS, CLIENT_ID);


int main(int argc, char const *argv[])
{   
    mqttInit();

    // Init Rover1
    std::string inputRover1 = R"({
    "sp": [null, null, null, null],
    "gp": [null, null, null, null],
    "gcp": [40, 150, 0],
    "v": [0, 0, 0],
    })";
    Rover rover1 = Rover(inputRover1);

    // Init Rover2
    std::string inputRover2 = R"({
    "sp": [null, null, null, null],
    "gp": [null, null, null, null],
    "gcp": [40, 20, 0],
    "v": [0, 0, 0],
    })";
    Rover rover2 = Rover(inputRover2);

    // pubMsg(cli);
    std::thread t1(pubMsg);
    std::thread t2(subMsg);

    t1.join();
    t2.join();

    // std::cout<<"Code"<<std::endl;
    return 0;
}


void mqttInit(){
    auto connOpts = mqtt::connect_options_builder()
    // .user_name("user")
    // .password("passwd")
    .keep_alive_interval(std::chrono::seconds(20))
    .automatic_reconnect(std::chrono::seconds(2), std::chrono::seconds(30))
    .clean_session(false)
    .finalize();

    std::cout << "Connecting to the MQTT server..." << std::flush;
    mqtt::connect_response rsp = cli.connect(connOpts);
    std::cout << "OK\n" << std::endl;

    const std::vector<std::string> TOPICS { "Map/Rover1/#", "Rover2/#" };
	const std::vector<int> QOS {0,0};


    if (!rsp.is_session_present()) {
        std::cout << "Subscribing to topics..." << std::flush;
        cli.subscribe(TOPICS, QOS);
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "Session already present. Skipping subscribe." << std::endl;
        std::cout << "Subscribing to topics..." << std::flush;
        cli.subscribe(TOPICS, QOS);
        std::cout << "OK" << std::endl;
    }

}


void pubMsg(){
    while (true){
        // std::unique_lock<std::mutex> lock(mtx); 
        // cv.wait(lock, []{ return publishing; });  
        std::cout << "\nSending message..." << std::endl;
        auto pubmsg = mqtt::make_message("Rover1", "halo");
        pubmsg->set_qos(0);
        cli.publish(pubmsg);
        // std::cout << "...OK" << std::endl;
        // publishing = false;
        // lock.unlock();
        // cv.notify_one();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void subMsg(){
    while (true) {
        // std::unique_lock<std::mutex> lock(mtx); 
        // cv.wait(lock, []{ return publishing; });  
        std::cout << "\nReciving message... \n" << std::endl;
        auto msg = cli.consume_message();
        if (msg) {
            if (msg->get_topic() == "command" &&
                    msg->to_string() == "exit") {
                std::cout << "Exit command received" << std::endl;
                break;
            }
            std::cout << msg->get_topic() << ": " << "Ini" << std::endl;

            std::cout << msg->get_topic() << ": " << msg->to_string() << std::endl;
            std::cout << msg->get_topic() << ": " << "ini" << std::endl;

        }
        else if (!cli.is_connected()) {
            std::cout << "Lost connection" << std::endl;
            while (!cli.is_connected()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }
            std::cout << "Re-established connection" << std::endl;
        }
        // publishing = true;
        // lock.unlock();
        // cv.notify_one();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
