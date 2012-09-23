#include "rss2parser.h"

#include <glibmm/convert.h>
#include <iostream>

const Glib::ustring RssParser::ItemLabel = "item";
const Glib::ustring RssParser::SummaryLabel = "description";
const Glib::ustring RssParser::ContentLabel = "content:encoded";
const Glib::ustring RssParser::PubDateLabel = "pubDate";
const Glib::ustring RssParser::UidLabel = "guid";
const Glib::ustring RssParser::ParserName = "RSS2 parser";

RssParser::RssParser() : 
  GenericParser()
{
}

RssParser::RssParser(AbstractDB * db, int id) :
  GenericParser(db, id)
{
}

RssParser::~RssParser() {
    
}

