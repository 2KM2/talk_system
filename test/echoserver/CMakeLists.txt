cmake_minimum_required(VERSION 2.5)
project(echo_server)

message(STATUS,"Begin echo_server")


message(STATUS,${LIBRARY_OUTPUT_PATH})

link_directories(${LIBRARY_OUTPUT_PATH})
add_executable(${PROJECT_NAME}  ${PROJECT_SOURCE_DIR}/main.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} common)
message(STATUS,"End echo_server")


