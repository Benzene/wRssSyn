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

add_executable (wRssSyn main.cpp rss2parser.cpp atomparser.cpp genericParser.cpp rssatomdecider.cpp tumblrparser.cpp tumblr.cpp tumblr_entry.cpp entry.cpp time_helpers.cpp auth.cpp db.cpp postgresdb.cpp curl_helpers.cpp)
target_link_libraries(wRssSyn ${LIBS})

INSTALL(PROGRAMS wRssSyn
    DESTINATION bin)
