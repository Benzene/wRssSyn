#ifndef MR_0710_ENTRY
#define MR_0710_ENTRY

#include "db.h"
#include <string>
#include <ctime>

class Entry {
  
  public:
    std::string title;
    std::string link;
    std::string description;
    std::string id;
    std::string date;
    
    Entry();
    Entry(std::string id, std::string title, 
	  std::string link, time_t date, 
	  std::string description);
    ~Entry();
    void print();
    void print_title();
    void write_to_db(AbstractDB * db, std::string website_id);
  
};

#endif
