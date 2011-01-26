#ifndef MR_0710_ENTRY
#define MR_0710_ENTRY

class Entry {
  
  public:
    std::string title;
    std::string link;
    std::string description;
    std::string id;
    std::string date;
    
    Entry();
    ~Entry();
    void print();
  
};

#endif
