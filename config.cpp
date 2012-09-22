#include "config.h"
#include <iostream>

using namespace std;

GenericConfig *Config::instance = NULL;

GenericConfig * Config::init() {

    map<string, string> defaults;
    defaults.insert(pair<string, string>(
                "useragent", "Mozilla/5.0 (compatible; wRssSyn crawler +https://rss.leukensis.org/crawler)"));
    defaults.insert(pair<string, string>(
                "db_name", "wrsssyn"));
    defaults.insert(pair<string, string>(
                "db_user", "wrsssyn"));
    defaults.insert(pair<string, string>(
                "db_pass", "wrsssyn"));

    GenericConfig * conf = new GenericConfig(defaults);

    conf->read_config("/etc/wRssSyn.conf");
    conf->read_config("/usr/local/etc/wRssSyn.conf");
    conf->read_config("wRssSyn.conf");

    return conf;
}

GenericConfig * Config::get_instance() {
    if (!instance) {
        instance = Config::init();
    }
    return instance;
}
