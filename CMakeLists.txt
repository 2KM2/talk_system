cmake_minimum_required(VERSION 2.5)

project(talk_system)


# 配置最终的可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/release/bin)
# 配置库输出位置
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/release/lib)


set(PROJECT_ROOT ${PROJECT_SOURCE_DIR})







# 配置头文件的搜索路径
include_directories(${PROJECT_SOURCE_DIR}/common/net)
include_directories(${PROJECT_SOURCE_DIR}/common/base)
include_directories(${PROJECT_SOURCE_DIR}/include)




set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -fPIC -pthread")



# 编译库
add_subdirectory(common)



# example

add_subdirectory(test)