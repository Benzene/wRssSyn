#ifndef MR_0710_TUMBLRPARSER
#define MR_0710_TUMBLRPARSER

#include <libxml++/libxml++.h>
#include "db.h"
#include "tumblr_entry.h"

class TumblrParser : public xmlpp::SaxParser
{
  public:
    TumblrParser();
    TumblrParser(AbstractDB * db);
    virtual ~TumblrParser();
    
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
    std::string entry_type;
    bool header_image;
    Glib::ustring CurrentTag;
    
    TumblrEntry * CurrentEntry;
    
    Glib::ustring GlobTitle;
    Glib::ustring GlobUrl;
    Glib::ustring GlobDescr;
    Glib::ustring ImgTitle;
    Glib::ustring ImgUrl;
    Glib::ustring ImgLink;
    
    AbstractDB * db;
    std::string id;
};

#endif
