#include <string>
#include "curl_helpers.h"

size_t store_data(char * ptr, size_t size, size_t nmemb, void * userdata) {
  std::string * storage = (std::string *)userdata;
  std::string str(ptr, size * nmemb);

  storage->append(str);

  return size * nmemb;
}
