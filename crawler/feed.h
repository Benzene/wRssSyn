#ifndef _WRSSSYN_FEED_H_
#define _WRSSSYN_FEED_H_

#include <string>

enum feed_type { FEED_UNK, FEED_RSS, FEED_TUMBLR };

struct feed {
  int uid;
  feed_type type;
  std::string feed_url;
  std::string title;
  std::string url;
  std::string descr;
  std::string etag;
  std::string lastmodified;
};

#endif
