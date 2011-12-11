#ifndef _WRSSSYN_TUMBLR_H_
#define _WRSSSYN_TUMBLR_H_
#include <cstdio>
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <curl/curl.h>
#include "db.h"

std::string * update_tumblr_feeds(AbstractDB * db);

#endif
