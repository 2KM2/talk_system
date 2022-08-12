cmake_minimum_required(VERSION 2.5)
project(test_log)

message(STATUS,"Begin  test_log")


#find_library(COMMON_LIB libcommon.so  ${EXECUTABLE_OUTPUT_PATH}/lib)
#IF (NOT COMMON_LIB)
#    MESSAGE(FATAL_ERROR "libcommon.so not found.")
#ENDIF(NOT COMMON_LIB)
message(STATUS,${LIBRARY_OUTPUT_PATH})
link_directories(${LIBRARY_OUTPUT_PATH})
add_executable(test_log  ${PROJECT_SOURCE_DIR}/test_log.cpp)
TARGET_LINK_LIBRARIES(test_log  common)
message(STATUS,"END  test_log")