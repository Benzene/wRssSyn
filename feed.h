#ifndef _WRSSSYN_FEED_H_
#define _WRSSSYN_FEED_H_

#include <string>

struct feed {
  std::string * id;
  std::string * feed_url;
  std::string * title;
  std::string * etag;
  std::string * lastmodified;
};

#endif
