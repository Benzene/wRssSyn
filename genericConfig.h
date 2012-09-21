#ifndef _WRSSSYN_GENERICCONFIG_H_
#define _WRSSSYN_GENERICCONFIG_H_

#include <string>
#include <map>

class GenericConfig {

    public:
        GenericConfig(std::map<std::string, std::string> default_values);
        GenericConfig(std::map<std::string, std::string> default_values, std::string filename);
        std::string val(std::string key);

    private:

        void setup_defaults(std::map<std::string, std::string> default_values);

        std::map<std::string, std::string> values;

        std::string trim_whitespace(std::string s);

};

#endif
