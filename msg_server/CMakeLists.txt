cmake_minimum_required(VERSION 2.5)
project(msgserver)



message(STATUS,"Begin ${PROJECT_NAME}")

include_directories(${PROJECT_ROOT}/include)
link_directories(${LIBRARY_OUTPUT_PATH}
        /usr/lib64/mysql/
)
find_library(MYSQL_LIB libmysqlclient.so /usr/lib64/mysql/)
IF (NOT MYSQL_LIB)
    MESSAGE(FATAL_ERROR "mysqlclient not found. If your mysql is mariadb, please install mariadb-devel otherwise install mysql-devel.")
ENDIF(NOT MYSQL_LIB)


set(msgserver_srcs
 ${PROJECT_SOURCE_DIR}/

)

add_executable(${PROJECT_NAME} ${chatserver_srcs})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} mysqlclient )


message(STATUS,"END ${PROJECT_NAME}")