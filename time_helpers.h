#ifndef MR_0710_TIMEHELPERS
#define MR_0710_TIMEHELPERS

#include <ctime>
#include <string>

class TimeHelpers {

  public:
    static time_t parseXMLtime(std::string str);
    static std::string convertInt(int n);
    
  private:
    static int str2int(const std::string str);

};

#endif
