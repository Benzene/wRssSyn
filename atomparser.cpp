#include "atomparser.h"

const Glib::ustring AtomParser::ItemLabel = "entry";
const Glib::ustring AtomParser::ContentLabel = "content";
const Glib::ustring AtomParser::PubDateLabel = "published";
const Glib::ustring AtomParser::UidLabel = "id";
const Glib::ustring AtomParser::ParserName = "ATOM parser";

AtomParser::AtomParser() :
  GenericParser()
{
}

AtomParser::AtomParser(sqlite3 * db, std::string id) : 
  GenericParser(db, id)
{
}

AtomParser::~AtomParser() {
    
}

