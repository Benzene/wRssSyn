#ifndef MR_0710_RSS2PARSER
#define MR_0710_RSS2PARSER

#include <libxml++/libxml++.h>
#include "entry.h"

class RssParser : public xmlpp::SaxParser
{
  public:
    RssParser();
    virtual ~RssParser();
    
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
    Glib::ustring CurrentTag;
    
    Entry * CurrentEntry;
    
    Glib::ustring GlobTitle;
    Glib::ustring GlobUrl;
    Glib::ustring GlobDescr;
};

#endif
