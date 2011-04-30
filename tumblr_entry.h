#ifndef MR_0710_TUMBLRENTRY
#define MR_0710_TUMBLRENTRY

#include <sqlite3.h>
#include <string>
#include <ctime>
#include <glibmm/ustring.h>
#include "entry.h"

class TumblrEntry : public Entry {
  
  public:
    std::string type;

    Glib::ustring poster_id;
    Glib::ustring photo_url;
    Glib::ustring photo_caption;
    Glib::ustring content_link;
    Glib::ustring quote_source;
    Glib::ustring question;
    Glib::ustring answer;
    
    TumblrEntry();
    ~TumblrEntry();
    void print();
    void write_to_db(sqlite3 * db, std::string website_id);
    Glib::ustring build_content();
  
};

#endif
