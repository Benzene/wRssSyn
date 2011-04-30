#ifndef MR_0710_RSSATOMDECIDER
#define MR_0710_RSSATOMDECIDER

#include <libxml++/libxml++.h>
#include <sqlite3.h>
#include "entry.h"

class RssAtomDecider : public xmlpp::SaxParser
{
  public:
    RssAtomDecider();
    virtual ~RssAtomDecider();

    bool is_atom();
    bool is_rss2();
    bool is_unknown();
   
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
    /* Describes the feed type : -1 for unknown, 0 for rss2, 1 for atom */
    int feed_type;
};

#endif
