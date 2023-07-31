project(rover)

add_library(rover rover.cpp)

target_include_directories(rover
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    )