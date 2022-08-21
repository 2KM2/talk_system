cmake_minimum_required(VERSION 2.5)
project(db_proxy_server)

message(STATUS,"Begin ${PROJECT_NAME}")

include_directories(${PROJECT_ROOT}/include)
link_directories(${LIBRARY_OUTPUT_PATH}
        /usr/lib64/mysql/
)
find_library(MYSQL_LIB libmysqlclient.so /usr/lib64/mysql/)
IF (NOT MYSQL_LIB)
    MESSAGE(FATAL_ERROR "mysqlclient not found. If your mysql is mariadb, please install mariadb-devel otherwise install mysql-devel.")
ENDIF(NOT MYSQL_LIB)


set(db_srcs
 ${PROJECT_SOURCE_DIR}/db_proxy_server.cc
 ${PROJECT_SOURCE_DIR}/DBPool.cc
 ${PROJECT_SOURCE_DIR}/CachePool.cc
 ${PROJECT_SOURCE_DIR}/DBServer.cc
)

add_executable(${PROJECT_NAME} ${db_srcs})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} mysqlclient common hiredis protobuf)


message(STATUS,"END ${PROJECT_NAME}")