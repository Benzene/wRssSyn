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
