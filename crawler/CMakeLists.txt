add_executable (wRssSyn main.cpp rss2parser.cpp atomparser.cpp genericParser.cpp rssatomdecider.cpp entry.cpp time_helpers.cpp db.cpp postgresdb.cpp curl_helpers.cpp genericConfig.cpp config.cpp)
target_link_libraries(wRssSyn ${LIBS})
