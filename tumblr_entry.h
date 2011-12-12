#ifndef MR_0710_TUMBLRENTRY
#define MR_0710_TUMBLRENTRY

#include <string>
#include "entry.h"

class TumblrEntry : public Entry {
  
  public:
    std::string type;

    std::string poster_id;
    std::string photo_url;
    std::string photo_caption;
    std::string content_link;
    std::string quote_source;
    std::string question;
    std::string answer;

    TumblrEntry();
    ~TumblrEntry();
    void print();
    std::string build_content();
  
};

#endif
