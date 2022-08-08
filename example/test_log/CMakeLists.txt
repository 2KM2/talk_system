cmake_minimum_required(VERSION 2.5)
project(test_log)
set(BIN_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set(COMMON_LIB_PAHT )
link_directories(
    ${PROJECT_SOURCE_DIR}/lib
   
)
include_directories( /home/zkm/talk_system/common)
find_library(COMMON_LIB libcommon.so  ${PROJECT_SOURCE_DIR}/lib)

IF (NOT COMMON_LIB)
    MESSAGE(FATAL_ERROR "mysqlclient not found. If your mysql is mariadb, please install mariadb-devel otherwise install mysql-devel.")
ENDIF(NOT COMMON_LIB)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -fPIC")


# ADD_DEFINITIONS(-DXXXV)
add_executable(test_log  ${PROJECT_SOURCE_DIR}/test_log.cpp)
TARGET_LINK_LIBRARIES(test_log  common)