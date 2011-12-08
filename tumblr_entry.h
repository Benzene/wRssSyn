#ifndef MR_0710_TUMBLRENTRY
#define MR_0710_TUMBLRENTRY

#include <string>
#include <ctime>
#include <glibmm/ustring.h>
#include "entry.h"
#include "db.h"

class TumblrEntry : public Entry {
  
  public:
    std::string type;

    /*
    Glib::ustring poster_id;
    Glib::ustring photo_url;
    Glib::ustring photo_caption;
    Glib::ustring content_link;
    Glib::ustring quote_source;
    Glib::ustring question;
    Glib::ustring answer;
    */
    
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
    void write_to_db(AbstractDB * db, std::string website_id);
    std::string build_content();
  
};

#endif
