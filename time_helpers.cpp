#include <iostream>
#include <sstream>
#include "time_helpers.h"

using namespace std;

int
TimeHelpers::str2int (const string str) {
  stringstream ss(str);
  int num;
  if((ss >> num).fail())
  { 
      cerr << "Error ! Malformed feed. Expected an int, got " << str << endl;
  }
  return num;
}

string 
TimeHelpers::convertInt(int number)
{
   stringstream ss;
   ss << number;
   return ss.str();
}

std::string
TimeHelpers::getPGREInput(time_t t) {
  struct tm * timeinfo = localtime(&t);
  std::string format = "%Y-%m-%d %H:%M:%S";
  char ptr[30];
  strftime(ptr, 30, format.c_str(), timeinfo);

  return std::string(ptr);
}

time_t
TimeHelpers::stampFromPGRE(std::string &str) {
  struct tm * timeinfo = new tm;
  time_t retValue = 0;
  std::string pgreFormat = "%Y-%m-%d %H:%M:%S";
  if (strptime(str.c_str(), pgreFormat.c_str(), timeinfo) == NULL) {
    std::cerr << "Time from pgre: " << str << " can't be converted to timestamp." << std::endl;
  } else {
    timeinfo->tm_isdst = -1;
    retValue = mktime(timeinfo);
  }

  delete timeinfo;

  return retValue;

}


time_t
TimeHelpers::parseXMLtime(const string str) {
  
  struct tm * timeinfo2 = new tm;
  time_t retValue2 = 0;
  string rss2format = "%a, %d %b %Y %H:%M:%S";
  string atomformat = "%Y-%m-%dT%H:%M:%S";
  if (strptime(str.substr(0,25).c_str(), rss2format.c_str(), timeinfo2) == NULL 
    && strptime(str.substr(0,19).c_str(), atomformat.c_str(), timeinfo2) == NULL) {
    cerr << "Time parsing failed !" << endl;
  } else {
    timeinfo2->tm_isdst = -1;
    retValue2 = mktime(timeinfo2);
  }
  delete timeinfo2;
  
//  cout << "strptime returns : " << asctime( localtime (&retValue2));
  
  return retValue2;
}
