#include "config.h"

using namespace std;

GenericConfig *Config::instance = NULL;

GenericConfig * Config::init() {

    string configFile = "wRssSyn.conf";

    map<string, string> defaults;
    defaults.insert(pair<string, string>(
                "useragent", "Mozilla/5.0 (compatible; wRssSyn crawler +https://rss.leukensis.org/crawler)"));
    defaults.insert(pair<string, string>(
                "db_name", "wrsssyn"));
    defaults.insert(pair<string, string>(
                "db_user", "wrsssyn"));
    defaults.insert(pair<string, string>(
                "db_pass", "wrsssyn"));

    return new GenericConfig(defaults, configFile);

}

GenericConfig * Config::get_instance() {
    if (!instance) {
        instance = Config::init();
    }
    return instance;
}
