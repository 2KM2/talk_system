cmake_minimum_required(VERSION 2.5)
project(test_threadpool)

message(STATUS,"Begin test_threadpool")


message(STATUS,${LIBRARY_OUTPUT_PATH})

link_directories(${LIBRARY_OUTPUT_PATH})
add_executable(${PROJECT_NAME}  ${PROJECT_SOURCE_DIR}/main.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} common)
message(STATUS,"End test_threadpool")


