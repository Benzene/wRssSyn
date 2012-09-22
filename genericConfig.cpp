#include "genericConfig.h"

#include <fstream>
#include <iostream>

GenericConfig::GenericConfig(std::map<std::string, std::string> default_values) : values(default_values) { }

void GenericConfig::read_config(const std::string& filename) {

    std::ifstream fin(filename.c_str());

    std::string line;
    while(std::getline(fin,line)) {

        // Remove comments
        size_t c = line.find('#');
        if (c != std::string::npos) {
            line = line.substr(0, c);
        }

        // Is there an '=' ?
        c = line.find('=');
        if (c != std::string::npos) {
            std::string key = trim_whitespace(line.substr(0, c));
            std::string value = trim_whitespace(line.substr(c+1));

            std::map<std::string, std::string>::iterator it = this->values.find(key);

            if (it != this->values.end()) {
                this->values.erase(it);
                values.insert(std::pair<std::string, std::string>(key,value));
            } else {
                std::cerr << "Warning ! Unknown configuration key: " << key << std::endl;
            }
        }
    }
}

std::string GenericConfig::val(const std::string& key) {

    std::map<std::string, std::string>::const_iterator it = this->values.find(key);

    if (it != this->values.end()) {
        return it->second;
    } else {
        return "";
    }
}

std::string GenericConfig::trim_whitespace(std::string s) {

    size_t begin = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");

    if (begin != std::string::npos) {
        if (end != std::string::npos) {
            return s.substr(begin, end-begin+1);
        } else {
            std::cerr << "Unreachable branch reached in trim_whitespace. You shouldn't be there." << std::endl;
            exit(-1);
        }
    } else {
        return "";
    }
}
