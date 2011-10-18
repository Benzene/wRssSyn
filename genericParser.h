#ifndef MR_0710_GENERICPARSER
#define MR_0710_GENERICPARSER

#include <libxml++/libxml++.h>
#include <sqlite3.h>
#include "entry.h"

class GenericParser : public xmlpp::SaxParser
{
  public:
    GenericParser();
    GenericParser(sqlite3 * db, std::string id);
    virtual ~GenericParser();

  protected:
    virtual void on_start_document();
    virtual void on_end_document();
    virtual void on_start_element(const Glib::ustring& name,
		    const AttributeList& properties);
    virtual void on_end_element(const Glib::ustring& name);
    virtual void on_cdata_block(const Glib::ustring& characters);
    virtual void on_characters(const Glib::ustring& characters);
    virtual void on_comment(const Glib::ustring& text);
    virtual void on_warning(const Glib::ustring& text);
    virtual void on_error(const Glib::ustring& text);
    virtual void on_fatal_error(const Glib::ustring& text);

    virtual const Glib::ustring& getItemLabel() =0;
    virtual const Glib::ustring& getContentLabel() =0;
    virtual const Glib::ustring& getPubDateLabel() =0;
    virtual const Glib::ustring& getUidLabel() =0;
    virtual const Glib::ustring& getParserName() =0;

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
