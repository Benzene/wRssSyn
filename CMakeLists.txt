cmake_minimum_required (VERSION 2.6)
project (wRssSyn)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(LibXML++ REQUIRED)
include_directories(${LibXML++_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LibXML++_LIBRARIES})

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})
set(LIBS ${LIBS} ${CURL_LIBRARIES})

#find_package(Sqlite3 REQUIRED)
#include_directories(${SQLITE3_INCLUDE_DIR})
#set(LIBS ${LIBS} ${SQLITE3_LIBRARIES})

find_package(Pqxx REQUIRED)
include_directories(${Pqxx_INCLUDE_DIR})
set(LIBS ${LIBS} ${Pqxx_LIBRARIES})

find_package(PostgreSQL REQUIRED)
include_directories(${PostgreSQL_INCLUDE_DIRS})
set(LIBS ${LIBS} ${PostgreSQL_LIBRARIES})
link_directories(${PostgreSQL_LIBRARY_DIR})

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-Wall")

add_subdirectory(crawler)
#add_executable (wRssSyn main.cpp rss2parser.cpp atomparser.cpp genericParser.cpp rssatomdecider.cpp entry.cpp time_helpers.cpp db.cpp postgresdb.cpp curl_helpers.cpp genericConfig.cpp config.cpp)
#target_link_libraries(wRssSyn ${LIBS})

INSTALL(PROGRAMS wRssSyn
    DESTINATION bin)
