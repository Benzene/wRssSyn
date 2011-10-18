#include "rss2parser.h"

const Glib::ustring RssParser::ItemLabel = "item";
const Glib::ustring RssParser::ContentLabel = "description";
const Glib::ustring RssParser::PubDateLabel = "pubDate";
const Glib::ustring RssParser::UidLabel = "guid";
const Glib::ustring RssParser::ParserName = "RSS2 parser";

RssParser::RssParser() : 
  GenericParser()
{
}

RssParser::RssParser(sqlite3 * db, std::string id) :
  GenericParser(db, id)
{
}

RssParser::~RssParser() {
    
}

