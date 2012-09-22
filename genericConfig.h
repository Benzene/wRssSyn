#ifndef _WRSSSYN_GENERICCONFIG_H_
#define _WRSSSYN_GENERICCONFIG_H_

#include <string>
#include <map>

class GenericConfig {

    public:
        GenericConfig(std::map<std::string, std::string> default_values);
        void read_config(const std::string& filename);
        std::string val(const std::string& key);

    private:

        std::map<std::string, std::string> values;

        std::string trim_whitespace(std::string s);

};

#endif
