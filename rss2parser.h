#ifndef MR_0710_RSS2PARSER
#define MR_0710_RSS2PARSER

#include <libxml++/libxml++.h>
#include <sqlite3.h>
#include "entry.h"
#include "genericParser.h"

class RssParser : public GenericParser
{
  public:
    RssParser();
    RssParser(sqlite3 * db, std::string id);
    virtual ~RssParser();
    
  protected:
    virtual const Glib::ustring& getItemLabel() { return ItemLabel; }
    virtual const Glib::ustring& getContentLabel() { return ContentLabel; }
    virtual const Glib::ustring& getPubDateLabel() { return PubDateLabel; }
    virtual const Glib::ustring& getUidLabel() { return UidLabel; }
    virtual const Glib::ustring& getParserName() { return ParserName; }

  private:
    static const Glib::ustring ItemLabel;
    static const Glib::ustring ContentLabel;
    static const Glib::ustring PubDateLabel;
    static const Glib::ustring UidLabel;
    static const Glib::ustring ParserName;

};

#endif
