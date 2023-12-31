#pragma once
#include <vector>
#include <cmath>
#include <json/json.h>

class Rover {
private:
    std::vector<float> robotState;
    // std::vector<float> sp{0.0, 0.0, 0.0, 0.0};
    std::vector<float> gp{0.0, 0.0, 0.0, 0.0};
    std::vector<float> obstacle;
    bool readyMove;

public:
    Rover(std::vector<float> &roverState, std::vector<float> &obstacle); 

    void updateState(std::vector<float> robotState);

    std::vector<float> getState();

    void setTask(std::string &inputTask); 

    std::vector<float> getTask();

    void setObstacle(const std::string& inputDataRover);

    std::vector<float> getObstacle();

    bool getReady();

    std::string getJSONString();

    void atGoal();

    // Print Vector
    void printVector(std::vector<float> vec);


    void display();

    //--------------------------------------------------------------  
    //Algoritma for velocity obstacle
    //print matrix
    // void printMatrix(const std::vector<std::vector<float>>& matrix);


    //element-wise multiplication of two matrices
    // std::vector<std::vector<float>> elementwise_multiply(const std::vector<std::vector<float>>& matrix1,
    //                                                     const std::vector<std::vector<float>>& matrix2);


    // Function to multiply two matrices
    // std::vector<std::vector<float>> matrix_multiply(const std::vector<std::vector<float>>& matrix1,
    //                                             const std::vector<std::vector<float>>& matrix2);


    // Function to multiply two matrices
    // std::vector<std::vector<float>> matrix_X_vector(const std::vector<std::vector<float>>& matrix1,
    //                                             const std::vector<float>& vector);
    
  
  
//   -------------------------------------------------------------------------------
  
  
    // //Transpose Matrix
    // std::vector<std::vector<float>> transposeMatrix(const std::vector<std::vector<float>>& matrix);
  
    // // Compute desired vel
    // std::vector<float> desired_velocity(const std::vector<float>& current_pos, const std::vector<float>& goal_pos);
 
    // // translate line
    // std::vector<float> translate_line(std::vector<float> line, std::vector<float> translation);

    // // create Constrain -> return A[0], A[1], b[0] 
    // std::vector<float> create_constraints(const std::vector<float>& translation, float angle);

    // // Checck constrain -> check inside
    // std::vector<std::vector<float>> check_constraints(const std::vector<std::vector<float>>& v,
    //                                             const std::vector<std::vector<float>>& Amat,
    //                                             const std::vector<float>& bvec);
    
    // //Compute Vel
    // std::vector<float> compute_velocity(const std::vector<float>& robot, const std::vector<float>& obstacles, std::vector<float>& v_desired);
 
    // std::vector<float> update_state(const std::vector<float>& x, const std::vector<float>& v);

    // void velocity_obstacle();

};