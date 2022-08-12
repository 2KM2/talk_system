cmake_minimum_required(VERSION 2.5)
project(common)
message(STATUS,"Begin common")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/base)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/net)


message(STATUS,${LIBRARY_OUTPUT_PATH} ${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -fPIC")

# 定义参与编译的源代码文件
aux_source_directory(${PROJECT_SOURCE_DIR}/base SRC_BASE)
aux_source_directory(${PROJECT_SOURCE_DIR}/net  SRC_NET)

# 编译生成动态库
add_library(common SHARED ${SRC_BASE} ${SRC_NET})


message(STATUS,"End common")