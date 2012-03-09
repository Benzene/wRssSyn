#include "rss2parser.h"

#include <glibmm/convert.h>
#include <iostream>

const Glib::ustring RssParser::ItemLabel = "item";
const Glib::ustring RssParser::ContentLabel = "description";
const Glib::ustring RssParser::PubDateLabel = "pubDate";
const Glib::ustring RssParser::UidLabel = "guid";
const Glib::ustring RssParser::ParserName = "RSS2 parser";

RssParser::RssParser() : 
  GenericParser()
{
}

RssParser::RssParser(AbstractDB * db, std::string id) :
  GenericParser(db, id)
{
}

RssParser::~RssParser() {
    
}

void
RssParser::on_cdata_block(const Glib::ustring& text) {
  try {
    if(in_entry && CurrentTag == "content:encoded") {
      CurrentEntry->description += text;
    }
  } catch (Glib::ConvertError e) {
    std::cerr << "Parsing error." << std::endl;
  }
}
	   
    
    
