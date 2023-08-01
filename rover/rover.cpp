#include <iostream>
#include <vector>
#include "json/json.h"
#include "rover.hpp"

// const float TIMESTEP = 0.1;
// const float ROBOT_RADIUS = 10.7;
// const int VMAX = 8; 

Rover::Rover(std::vector<float> &robotState, std::vector<float> &obstacle){
    this->robotState = robotState;
    this->obstacle = obstacle;
    readyMove = false;
}


void Rover::updateState(std::vector<float> robotState) {
    // std::cout << "Updating Rover State..." << std::endl;
    this->robotState = robotState;
}


std::vector<float> Rover::getState(){
    return robotState;
}


void Rover::setTask(std::string &inputTask) {
    Json::CharReaderBuilder builderTask;
    Json::Value task;
    Json::String errsTask;
    std::istringstream jsonStream(inputTask);
    bool parsingSuccessful = Json::parseFromStream(builderTask, jsonStream, &task, &errsTask);
    if (!parsingSuccessful) {
        std::cout << "Failed to parse JSON task: " << errsTask << std::endl;
    }
    else{
        std::cout << "Setting Rover Task..." << std::endl;
        const Json::Value& spArray = task["sp"];
        for (int i = 0; i < spArray.size(); ++i) {
            // Update values of vector sp and JSON data Rover
            sp[i] = spArray[i].asFloat();
            // std::cout<<spArray[i]<<std::endl;
        }
        const Json::Value& gpArray = task["gp"];
        for (int i = 0; i < gpArray.size(); ++i) {
            // Update values of vector gp and JSON data Rover
            gp[i] = gpArray[i].asFloat();
            // std::cout<<gpArray[i]<<std::endl;
        }
        readyMove = true;
    }
}


std::vector<float> Rover::getTask(){
    return gp;
}


void Rover::setObstacle(const std::string& inputObstacle) {
    Json::CharReaderBuilder builderObstacle;
    Json::Value dataObstacle;
    Json::String errsObstacle;
    std::istringstream jsonStream(inputObstacle);
    bool parsingSuccessful = Json::parseFromStream(builderObstacle, jsonStream, &dataObstacle, &errsObstacle);
    if (!parsingSuccessful) {
        std::cout << "Failed to parse JSON obstacle: " << errsObstacle << std::endl;
    }

    else{
        const Json::Value& robotStateArray = dataObstacle["robot_state"];
        for (int i = 0; i < robotStateArray.size()-1; ++i) {
            obstacle[i] = robotStateArray[i].asFloat();
        }
    }
}


std::vector<float> Rover::getObstacle(){
    return obstacle;
}


bool Rover::getReady(){
    return readyMove;
}


std::string Rover::getJSONString() {
    // Create a JSON object
    Json::Value jsonObj;
    // Array 
    Json::Value robot_state;
    robot_state.append(robotState[0]);
    robot_state.append(robotState[1]);
    robot_state.append(robotState[2]);
    robot_state.append(robotState[3]);
    jsonObj["robot_state"] = robot_state;
    // Serialize the JSON object to a string
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "\t";
    std::string jsonString = Json::writeString(writer, jsonObj);
    // std::cout << " JSON String = " << jsonString << std::endl;
    return jsonString;
}

// Print Vector
// void Rover::printVector(std::vector<int> vec){
//         for (int i = 0; i < vec.size(); ++i) {
//         std::cout << vec[i] << " ";
//     }
//     std::cout << std::endl;
// }

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
    std::cout << " obstacle     = ";
    printVector(obstacle);
    // std::cout << " Data Rover   = " << dataRover << std::endl;
    // std::cout << std::endl;
}


// Add library Velocity obstacle 
//print vector 2D
// void Rover::printMatrix(const std::vector<std::vector<float>>& matrix){
//     for ( const auto &row : matrix ){
//         for ( const auto &s : row ) std::cout << s << ' ';
//         std::cout << std::endl;
//     }
// }

//element-wise multiplication of two matrices
// std::vector<std::vector<float>> Rover::elementwise_multiply(const std::vector<std::vector<float>>& matrix1,
//                                                      const std::vector<std::vector<float>>& matrix2) {
//     int rows = matrix1.size();
//     int cols = matrix1[0].size();

//     // Initialize the result matrix with zeros
//     std::vector<std::vector<float>> result(rows, std::vector<float>(cols, 0.0));

//     // Perform element-wise multiplication
//     for (int i = 0; i < rows; ++i) {
//         for (int j = 0; j < cols; ++j) {
//             result[i][j] = matrix1[i][j] * matrix2[i][j];
//         }
//     }

//     return result;
// }

// Function to multiply two matrices
// std::vector<std::vector<float>> Rover::matrix_multiply(const std::vector<std::vector<float>>& matrix1,
//                                                const std::vector<std::vector<float>>& matrix2) {
//     int rows1 = matrix1.size();
//     int cols1 = matrix1[0].size();
//     int rows2 = matrix2.size();
//     int cols2 = matrix2[0].size();

//     if (cols1 != rows2) {
//         throw std::invalid_argument("Matrix dimensions are not compatible for multiplication.");
//     }

//     // Initialize the result matrix with zeros
//     std::vector<std::vector<float>> result(rows1, std::vector<float>(cols2, 0.0));

//     // Perform matrix multiplication
//     for (int i = 0; i < rows1; ++i) {
//         for (int j = 0; j < cols2; ++j) {
//             for (int k = 0; k < cols1; ++k) {
//                 result[i][j] += matrix1[i][k] * matrix2[k][j];
//             }
//         }
//     }

//     return result;
// }

// Function to multiply two matrices
// std::vector<std::vector<float>> Rover::matrix_X_vector(const std::vector<std::vector<float>>& matrix1,
//                                                const std::vector<float>& vector) {
//     int rows1 = matrix1.size();
//     int cols1 = matrix1[0].size();
//     int cols2 = vector.size();

//     if (cols1 != 1) {
//         throw std::invalid_argument("Matrix dimensions are not compatible for multiplication.");
//     }

//     // Initialize the result matrix with zeros
//     std::vector<std::vector<float>> result(rows1, std::vector<float>(cols2, 0.0));

//     // Perform matrix multiplication
//     for (int i = 0; i < rows1; ++i) {
//         for (int j = 0; j < cols2; ++j) {
//             for (int k = 0; k < cols1; ++k) {
//                 result[i][j] += matrix1[i][k] * vector[k];
//             }
//         }
//     }

//     return result;
// }


// ---------------------------------------------------------------------------------


// //Transpose Matrix
// std::vector<std::vector<float>> Rover::transposeMatrix(const std::vector<std::vector<float>>& matrix) {
//     int rows = matrix.size();
//     int cols = matrix[0].size();

//     // Create a new matrix to store the transposed matrix
//     std::vector<std::vector<float>> transposed(cols, std::vector<float>(rows));

//     // Transpose the matrix
//     for (int i = 0; i < rows; ++i) {
//         for (int j = 0; j < cols; ++j) {
//             transposed[j][i] = matrix[i][j];
//         }
//     }

//     return transposed;
// }

// // Compute desired vel
// std::vector<float> Rover::desired_velocity(const std::vector<float>& current_pos, const std::vector<float>& goal_pos) {
  
//     std::vector<float> disp_vec = {goal_pos[0] - current_pos[0], goal_pos[1] - current_pos[1]};
//     float norm = std::sqrt(disp_vec[0] * disp_vec[0] + disp_vec[1] * disp_vec[1]);

//     if (norm <= ROBOT_RADIUS / 13) {
//         return std::vector<float>{0.0, 0.0};
//     }

//     disp_vec[0] /= norm;
//     disp_vec[1] /= norm;

//     std::vector<float> desired_vel = {VMAX * disp_vec[0], VMAX * disp_vec[1]};
//     // Uncomment the following line to print desired velocity
//     // std::cout << "desired velocity = " << desired_vel[0] << ", " << desired_vel[1] << std::endl;
//     return desired_vel;
// }
// // translate line
// std::vector<float> Rover::translate_line(std::vector<float> line, std::vector<float> translation) {
//     // Buat matriks identitas 3x3
//     std::vector<std::vector<float>> matrix(3, std::vector<float>(3, 0.0));
//     for (int i = 0; i < 3; ++i) {
//         matrix[i][i] = 1.0;
//     }
    
//     // Ubah elemen baris terakhir pada matriks dengan -translation
//     matrix[2][0] = -translation[0];
//     matrix[2][1] = -translation[1];

//     // Lakukan perkalian antara matriks dan line
//     std::vector<float> result(3, 0.0);
//     for (int i = 0; i < 3; ++i) {
//         for (int j = 0; j < 3; ++j) {
//             result[i] += matrix[i][j] * line[j];
//         }
//     }

//     return result;
// }

// // create Constrain -> return A[0], A[1], b[0] 
// std::vector<float> Rover::create_constraints(const std::vector<float>& translation, float angle) {
// // std::vector<float> create_constraints(const std::vector<float>& translation, float angle, const char* side) {

//     // Membuat line
//     std::vector<float> origin = {0.0, 0.0, 1.0};
//     std::vector<float> point = {std::cos(angle), std::sin(angle)};
//     std::vector<float> line = {(origin[1] * point[2]) - (origin[2] * point[1]),
//                                (origin[2] * point[0]) - (origin[0] * point[2]),
//                                (origin[0] * point[1]) - (origin[1] * point[0])};
    
//     line = translate_line(line, translation);


//     // Ekstrak koefisien A dan b
//     std::vector<float> A = {line[0], line[1]};
//     float b = -line[2];

//     // return {A, b};
//     return {A[0], A[1], b};
// }

// // Checck constrain -> check inside
// std::vector<std::vector<float>> Rover::check_constraints(const std::vector<std::vector<float>>& v,
//                                              const std::vector<std::vector<float>>& Amat,
//                                              const std::vector<float>& bvec) {
//     // std::cout << "V Sample ada = " << v.size() << " bar dan " << v[0].size() << " kol." << std::endl;
//     std::vector<std::vector<float>> v_out;
//         for (int i = 0; i < v[0].size(); ++i) {
//             bool row1,row2;
//             row1 = (Amat[0][0]*v[0][i] + Amat[0][1]*v[1][i]) > bvec[0];
//             row2 = (Amat[1][0]*v[0][i] + Amat[1][1]*v[1][i]) < bvec[1];
//             if(not(row1 && row2)){
//                 v_out.push_back({v[0][i], v[1][i]});
//             }
//         }
//     v_out = transposeMatrix(v_out);
//     return v_out;
// }


// std::vector<float> Rover::compute_velocity(const std::vector<float>& robot, const std::vector<float>& obstacles, std::vector<float>& v_desired) {
//     // Make list for current Rover [pX, pY, vX, vY]
//     std::vector<float> pA = {robot[0], robot[1]};
//     std::vector<float> vA = {robot[2], robot[3]};
//     // Compute the constraints
//     // for each velocity obstacles
//     std::vector<std::vector<float>> Amat(2, std::vector<float>(2));
//     std::vector<float> bvec(2);
    
//     std::vector<float> pB = {obstacles[0], obstacles[1]};
//     std::vector<float> vB = {obstacles[2], obstacles[3]};
//     std::vector<float> dispBA(2);
//     dispBA[0] = pA[0] - pB[0];
//     dispBA[1] = pA[1] - pB[1];
//     float distBA = std::sqrt(dispBA[0] * dispBA[0] + dispBA[1] * dispBA[1]);
//     float thetaBA = std::atan2(dispBA[1], dispBA[0]);
//     if (2.2 * ROBOT_RADIUS > distBA) {
//         distBA = 2.2 * ROBOT_RADIUS;
//     }
//     float phi_obst = std::asin(2.2 * ROBOT_RADIUS / distBA);
//     float phi_left = thetaBA + phi_obst;
//     float phi_right = thetaBA - phi_obst;

//     // VO
//     std::vector<float> temp(3);
//     temp = create_constraints(vB, phi_left);
//     Amat[0][0] = temp[0];
//     Amat[0][1] = temp[1];
//     bvec[0] = temp[2];
//     temp = create_constraints(vB, phi_right);
//     Amat[1][0] = temp[0];
//     Amat[1][1] = temp[1];
//     bvec[1] = temp[2];

//     // Create search-space
//     const int num_th = 20;
//     const int num_vel = 32;
//     std::vector<float> th(num_th);
//     std::vector<float> vel(num_vel);
//     for (int i = 0; i < num_th; ++i) {
//         th[i] = i * 2 * M_PI / (num_th - 1);
//     }
//     for (int i = 0; i < num_vel; ++i) {
//         vel[i] = i * VMAX / (num_vel - 1);
//     }

//     std::vector<std::vector<float>> vv(num_th, std::vector<float>(num_vel));
//     std::vector<std::vector<float>> thth(num_th, std::vector<float>(num_vel));
//     for (int i = 0; i < num_th; ++i) {
//         for (int j = 0; j < num_vel; ++j) {
//             vv[i][j] = vel[j];
//             thth[i][j] = th[i];
//         }
//     }

//     std::vector<float> vx_sample(num_th * num_vel);
//     std::vector<float> vy_sample(num_th * num_vel);
//     for (int i = 0; i < num_th; ++i) {
//         for (int j = 0; j < num_vel; ++j) {
//             vx_sample[i * num_vel + j] = vv[i][j] * std::cos(thth[i][j]);
//             vy_sample[i * num_vel + j] = vv[i][j] * std::sin(thth[i][j]);
//         }
//     }

//     // Stack Array
//     // [[vx],
//     //  [vy]]
//     std::vector<std::vector<float>> v_sample(2, std::vector<float>(num_th * num_vel));
//     for (int i = 0; i < num_th * num_vel; ++i) {
//         v_sample[0][i] = vx_sample[i];
//         v_sample[1][i] = vy_sample[i];
//     }

//     std::vector<std::vector<float>> v_satisfying_constraints = check_constraints(v_sample, Amat, bvec);

//     // Objective function
//     int size = v_satisfying_constraints[0].size();
//     std::vector<std::vector<float>> diffs(2, std::vector<float>(size));
//     for (int col = 0; col < size; ++col) {
//             diffs[0][col] = v_satisfying_constraints[0][col] - v_desired[0];
//             diffs[1][col] = v_satisfying_constraints[1][col] - v_desired[1];
//         }


//     std::vector<float> norm(size);
//     for (int i = 0; i < size; ++i) {
//         norm[i] = std::sqrt(diffs[0][i] * diffs[0][i] + diffs[1][i] * diffs[1][i]);
//     }

//     int min_index = 0;
//     for (int i = 1; i < size; ++i) {
//         if (norm[i] < norm[min_index]) {
//             min_index = i;
//         }
//     }

//     std::vector<float> cmd_vel(2);
//     cmd_vel[0] = v_satisfying_constraints[0][min_index];
//     cmd_vel[1] = v_satisfying_constraints[1][min_index];
//     return cmd_vel;
// }

// std::vector<float> Rover::update_state(const std::vector<float>& xy, const std::vector<float>& v) {
//     // This state contains (x, y, vx, vy)
//     std::vector<float> new_state(4);
//     new_state[0] = xy[0] + v[0] * TIMESTEP;
//     new_state[1] = xy[1] + v[1] * TIMESTEP;
//     new_state[2] = v[0];
//     new_state[3] = v[1];

//     updateState(new_state);
//     return new_state;
// }


// void Rover::velocity_obstacle(){
//     if(readyMove){
//         std::vector<float> v_desired = desired_velocity(robotState, gp);

//         // Compute control velocity
//         std::vector<float> control_vel = compute_velocity(robotState, obstacle, v_desired);

//         // Update robot state
//         robotState = update_state(robotState, control_vel);
//         std::cout << "Robot State  = ";
//         printVector(robotState);
//         if(robotState[2]==0 && robotState[3]==0){
//             // std::cout << "Goal coordinate " << robotState << ", " << task[1] << std::endl;
//             std::cout << "Rover arrived at the goal" << std::endl;
//             readyMove=0;
//         }
//     }
// }
