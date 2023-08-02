#include <iostream>
#include <chrono>
#include <string>
#include <thread>
#include <vector>
#include "mqtt/async_client.h"
#include "mqtt/client.h"
#include "json/json.h"
#include "rover/rover.hpp"


const std::string SERVER_ADDRESS	{ "localhost:1883" };
const std::string CLIENT_ID		    { "Rover2" };
const float TIMESTEP = 0.1;
const float ROBOT_RADIUS = 10.7;
const int VMAX = 8; 

void mqttInit(mqtt::client &cli);
void pubMsg(mqtt::client &cli, Rover& rover2);
void subMsg(mqtt::client &cli, Rover& rover2) ;
// Algoritma VO
//Transpose Matrix
std::vector<std::vector<float>> transposeMatrix(const std::vector<std::vector<float>> &matrix);

// Compute desired vel
std::vector<float> desired_velocity(const std::vector<float> &current_pos, const std::vector<float> &goal_pos);

// translate line
std::vector<float> translate_line(std::vector<float> line, std::vector<float> translation);

// create Constrain -> return A[0], A[1], b[0] 
std::vector<float> create_constraints(const std::vector<float> &translation, float angle);

// Checck constrain -> check inside
std::vector<std::vector<float>> check_constraints(const std::vector<std::vector<float>> &v,
                                            const std::vector<std::vector<float>> &Amat,
                                            const std::vector<float> &bvec);

//Compute Vel
std::vector<float> compute_velocity(const std::vector<float> &robot, const std::vector<float> &obstacles, std::vector<float> &v_desired);

std::vector<float> update_state(const std::vector<float> &x, const std::vector<float> &v);

// void velocity_obstacle();

int main(int argc, char const *argv[])
{   
    mqtt::client cli(SERVER_ADDRESS, CLIENT_ID);
    mqttInit(cli);

    // Init Rover2
    std::vector<float> robotState{40, 20, 0, 0};
    std::vector<float> obstacle{40, 150, 0, 0};

    Rover rover2 = Rover(robotState, obstacle);

    std::thread t1(pubMsg,std::ref(cli), std::ref(rover2));
    std::thread t2(subMsg,std::ref(cli), std::ref(rover2));

    t1.join();
    t2.join();

    return 0;
}


void mqttInit(mqtt::client &cli){
    auto connOpts = mqtt::connect_options_builder()
    .keep_alive_interval(std::chrono::seconds(30))
    .automatic_reconnect(std::chrono::seconds(2), std::chrono::seconds(30))
    .clean_session(true)
    .finalize();

    std::cout << "Connecting to the MQTT server..." << std::flush;
    mqtt::connect_response rsp = cli.connect(connOpts);
    std::cout << "OK\n" << std::endl;

    const std::vector<std::string> TOPICS { "Map/Rover1/State", "Map/Rover2/#", "Map/Simulasi/#"};
    const std::vector<int> QOS {0, 0, 0};

    std::cout << "Subscribing to topics..." << std::flush;
    cli.subscribe(TOPICS, QOS);
    std::cout << "OK" << std::endl; 
}


void pubMsg(mqtt::client &cli, Rover &rover2){
    while (true){
        std::string msg = rover2.getJSONString();
        auto pubmsg = mqtt::make_message("Map/Rover2/State", msg);
        pubmsg->set_qos(0);
        cli.publish(pubmsg);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void subMsg(mqtt::client &cli, Rover &rover2){
    while (true) {
        auto msg = cli.consume_message();
        if (msg) {
            // std::cout << msg->get_topic() << ": " << msg->to_string() << std::endl;
            if (msg->get_topic() == "Map/Rover2/Task"){
                std::string msgString = msg->to_string();
                rover2.setTask(msgString); 
            }
            if (msg->get_topic() == "Map/Simulasi/Command" && msg->to_string() == "display") {
                rover2.display();
            }
            if (msg->get_topic() == "Map/Rover1/State"){
                std::string msgString = msg->to_string();
                rover2.setObstacle(msgString); 
            }
            bool readyMove = rover2.getReady();
            if(readyMove){
                std::vector<float> robotState = rover2.getState();
                std::vector<float> gp = rover2.getTask();
                std::vector<float> v_desired = desired_velocity(robotState, gp);

                // Compute control velocity
                std::vector<float> obstacle = rover2.getObstacle();
                std::vector<float> control_vel = compute_velocity(robotState, obstacle, v_desired);

                // Update robot state
                robotState = update_state(robotState, control_vel);
                std::cout << "Robot State  = ";
                rover2.printVector(robotState);
                if(robotState[2]==0 && robotState[3]==0){
                    std::cout << "Rover arrived at the goal" << std::endl;
                    readyMove=0;
                }
            }

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


//Transpose Matrix
std::vector<std::vector<float>> transposeMatrix(const std::vector<std::vector<float>> &matrix) {
    int rows = matrix.size();
    int cols = matrix[0].size();

    // Create a new matrix to store the transposed matrix
    std::vector<std::vector<float>> transposed(cols, std::vector<float>(rows));

    // Transpose the matrix
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            transposed[j][i] = matrix[i][j];
        }
    }

    return transposed;
}

// Compute desired vel
std::vector<float> desired_velocity(const std::vector<float> &current_pos, const std::vector<float> &goal_pos) {
  
    std::vector<float> disp_vec = {goal_pos[0] - current_pos[0], goal_pos[1] - current_pos[1]};
    float norm = std::sqrt(disp_vec[0] * disp_vec[0] + disp_vec[1] * disp_vec[1]);

    if (norm <= ROBOT_RADIUS / 13) {
        return std::vector<float>{0.0, 0.0};
    }

    disp_vec[0] /= norm;
    disp_vec[1] /= norm;

    std::vector<float> desired_vel = {VMAX * disp_vec[0], VMAX * disp_vec[1]};
    // Uncomment the following line to print desired velocity
    // std::cout << "desired velocity = " << desired_vel[0] << ", " << desired_vel[1] << std::endl;
    return desired_vel;
}
// translate line
std::vector<float> translate_line(std::vector<float> line, std::vector<float> translation) {
    // Buat matriks identitas 3x3
    std::vector<std::vector<float>> matrix(3, std::vector<float>(3, 0.0));
    for (int i = 0; i < 3; ++i) {
        matrix[i][i] = 1.0;
    }
    
    // Ubah elemen baris terakhir pada matriks dengan -translation
    matrix[2][0] = -translation[0];
    matrix[2][1] = -translation[1];

    // Lakukan perkalian antara matriks dan line
    std::vector<float> result(3, 0.0);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            result[i] += matrix[i][j] * line[j];
        }
    }

    return result;
}

// create Constrain -> return A[0], A[1], b[0] 
std::vector<float> create_constraints(const std::vector<float> &translation, float angle) {
    // Create line
    std::vector<float> origin = {0.0, 0.0, 1.0};
    std::vector<float> point = {std::cos(angle), std::sin(angle)};
    std::vector<float> line = {(origin[1] * point[2]) - (origin[2] * point[1]),
                               (origin[2] * point[0]) - (origin[0] * point[2]),
                               (origin[0] * point[1]) - (origin[1] * point[0])};
    
    line = translate_line(line, translation);


    // Ekstrak koefisien A dan b
    std::vector<float> A = {line[0], line[1]};
    float b = -line[2];

    // return {A, b};
    return {A[0], A[1], b};
}

// Checck constrain -> check inside
std::vector<std::vector<float>> check_constraints(const std::vector<std::vector<float>> &v,
                                             const std::vector<std::vector<float>> &Amat,
                                             const std::vector<float> &bvec) {
    // std::cout << "V Sample ada = " << v.size() << " bar dan " << v[0].size() << " kol." << std::endl;
    std::vector<std::vector<float>> v_out;
        for (int i = 0; i < v[0].size(); ++i) {
            bool row1,row2;
            row1 = (Amat[0][0]*v[0][i] + Amat[0][1]*v[1][i]) > bvec[0];
            row2 = (Amat[1][0]*v[0][i] + Amat[1][1]*v[1][i]) < bvec[1];
            if(not(row1 && row2)){
                v_out.push_back({v[0][i], v[1][i]});
            }
        }
    v_out = transposeMatrix(v_out);
    return v_out;
}


std::vector<float> compute_velocity(const std::vector<float>& robot, const std::vector<float> &obstacles, std::vector<float> &v_desired) {
    // Make list for current Rover [pX, pY, vX, vY]
    std::vector<float> pA = {robot[0], robot[1]};
    std::vector<float> vA = {robot[2], robot[3]};
    // Compute the constraints
    // for each velocity obstacles
    std::vector<std::vector<float>> Amat(2, std::vector<float>(2));
    std::vector<float> bvec(2);
    
    std::vector<float> pB = {obstacles[0], obstacles[1]};
    std::vector<float> vB = {obstacles[2], obstacles[3]};
    std::vector<float> dispBA(2);
    dispBA[0] = pA[0] - pB[0];
    dispBA[1] = pA[1] - pB[1];
    float distBA = std::sqrt(dispBA[0] * dispBA[0] + dispBA[1] * dispBA[1]);
    float thetaBA = std::atan2(dispBA[1], dispBA[0]);
    if (2.2 * ROBOT_RADIUS > distBA) {
        distBA = 2.2 * ROBOT_RADIUS;
    }
    float phi_obst = std::asin(2.2 * ROBOT_RADIUS / distBA);
    float phi_left = thetaBA + phi_obst;
    float phi_right = thetaBA - phi_obst;

    // VO
    std::vector<float> temp(3);
    temp = create_constraints(vB, phi_left);
    Amat[0][0] = temp[0];
    Amat[0][1] = temp[1];
    bvec[0] = temp[2];
    temp = create_constraints(vB, phi_right);
    Amat[1][0] = temp[0];
    Amat[1][1] = temp[1];
    bvec[1] = temp[2];

    // Create search-space
    const int num_th = 20;
    const int num_vel = 32;
    std::vector<float> th(num_th);
    std::vector<float> vel(num_vel);
    for (int i = 0; i < num_th; ++i) {
        th[i] = i * 2 * M_PI / (num_th - 1);
    }
    for (int i = 0; i < num_vel; ++i) {
        vel[i] = i * VMAX / (num_vel - 1);
    }

    std::vector<std::vector<float>> vv(num_th, std::vector<float>(num_vel));
    std::vector<std::vector<float>> thth(num_th, std::vector<float>(num_vel));
    for (int i = 0; i < num_th; ++i) {
        for (int j = 0; j < num_vel; ++j) {
            vv[i][j] = vel[j];
            thth[i][j] = th[i];
        }
    }

    std::vector<float> vx_sample(num_th * num_vel);
    std::vector<float> vy_sample(num_th * num_vel);
    for (int i = 0; i < num_th; ++i) {
        for (int j = 0; j < num_vel; ++j) {
            vx_sample[i * num_vel + j] = vv[i][j] * std::cos(thth[i][j]);
            vy_sample[i * num_vel + j] = vv[i][j] * std::sin(thth[i][j]);
        }
    }

    // Stack Array
    // [[vx],
    //  [vy]]
    std::vector<std::vector<float>> v_sample(2, std::vector<float>(num_th * num_vel));
    for (int i = 0; i < num_th * num_vel; ++i) {
        v_sample[0][i] = vx_sample[i];
        v_sample[1][i] = vy_sample[i];
    }

    std::vector<std::vector<float>> v_satisfying_constraints = check_constraints(v_sample, Amat, bvec);

    // Objective function
    int size = v_satisfying_constraints[0].size();
    std::vector<std::vector<float>> diffs(2, std::vector<float>(size));
    for (int col = 0; col < size; ++col) {
            diffs[0][col] = v_satisfying_constraints[0][col] - v_desired[0];
            diffs[1][col] = v_satisfying_constraints[1][col] - v_desired[1];
        }


    std::vector<float> norm(size);
    for (int i = 0; i < size; ++i) {
        norm[i] = std::sqrt(diffs[0][i] * diffs[0][i] + diffs[1][i] * diffs[1][i]);
    }

    int min_index = 0;
    for (int i = 1; i < size; ++i) {
        if (norm[i] < norm[min_index]) {
            min_index = i;
        }
    }

    std::vector<float> cmd_vel(2);
    cmd_vel[0] = v_satisfying_constraints[0][min_index];
    cmd_vel[1] = v_satisfying_constraints[1][min_index];
    return cmd_vel;
}

std::vector<float> update_state(const std::vector<float> &xy, const std::vector<float> &v) {
    // This state contains (x, y, vx, vy)
    std::vector<float> new_state(4);
    new_state[0] = xy[0] + v[0] * TIMESTEP;
    new_state[1] = xy[1] + v[1] * TIMESTEP;
    new_state[2] = v[0];
    new_state[3] = v[1];

    // updateState(new_state);
    return new_state;
}