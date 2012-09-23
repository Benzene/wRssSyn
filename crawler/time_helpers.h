#ifndef MR_0710_TIMEHELPERS
#define MR_0710_TIMEHELPERS

#include <ctime>
#include <string>

class TimeHelpers {

  public:
    static time_t parseXMLtime(std::string str);
    static std::string convertInt(int n);
    static std::string getPGREInput(time_t t);
    static time_t stampFromPGRE(std::string &str);
    static int str2int(const std::string str);

};

#endif
