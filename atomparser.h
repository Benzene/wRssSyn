#ifndef MR_0710_ATOMPARSER
#define MR_0710_ATOMPARSER

#include <libxml++/libxml++.h>
#include <sqlite3.h>
#include "entry.h"

class AtomParser : public xmlpp::SaxParser
{
  public:
    AtomParser();
    AtomParser(sqlite3 * db, std::string id);
    virtual ~AtomParser();
    
  protected:
    virtual void on_start_document();
    virtual void on_end_document();
    virtual void on_start_element(const Glib::ustring& name,
				  const AttributeList& properties);
    virtual void on_end_element(const Glib::ustring& name);
    virtual void on_characters(const Glib::ustring& characters);
    virtual void on_comment(const Glib::ustring& text);
    virtual void on_warning(const Glib::ustring& text);
    virtual void on_error(const Glib::ustring& text);
    virtual void on_fatal_error(const Glib::ustring& text);
    
  private:
    bool in_entry;
    bool header_image;
    Glib::ustring CurrentTag;
    
    Entry * CurrentEntry;
    
    Glib::ustring GlobTitle;
    Glib::ustring GlobUrl;
    Glib::ustring GlobDescr;
    Glib::ustring ImgTitle;
    Glib::ustring ImgUrl;
    Glib::ustring ImgLink;
    
    sqlite3 * db;
    std::string id;
};

#endif
