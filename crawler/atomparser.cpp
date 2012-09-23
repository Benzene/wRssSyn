#include "atomparser.h"

#include <glibmm/convert.h>
#include <iostream>

const Glib::ustring AtomParser::ItemLabel = "entry";
const Glib::ustring AtomParser::SummaryLabel = "summary";
const Glib::ustring AtomParser::ContentLabel = "content";
const Glib::ustring AtomParser::PubDateLabel = "published";
const Glib::ustring AtomParser::UidLabel = "id";
const Glib::ustring AtomParser::ParserName = "ATOM parser";

AtomParser::AtomParser() :
  GenericParser()
{
}

AtomParser::AtomParser(AbstractDB * db, int id) : 
  GenericParser(db, id)
{
}

AtomParser::~AtomParser() {
    
}

void
AtomParser::on_start_element(const Glib::ustring& name, const AttributeList& attributes) {
  // Special case, a link in an atom feed is in the attributes.
  if (name == "link") {
    Glib::ustring type;
    Glib::ustring href;
    Glib::ustring rel;

    AttributeList::const_iterator it;
    for (it = attributes.begin(); it != attributes.end(); ++it) {
      if( (*it).name == "type") {
        type = (*it).value;
      } else if( (*it).name == "href") {
        href = (*it).value;
      } else if( (*it).name == "rel") {
        rel = (*it).value;
      }
    }

    if (rel == "self") {
      // Url of the atom feed. -> ?
    } else if ((rel == "alternate" || rel == "") && (type == "text/html" || type == "")) {
      if (!in_entry) {
        GlobUrl = href;
      } else {
        CurrentEntry->link = href;
      }
    }
  } else {
    GenericParser::on_start_element(name, attributes);
  }

  if (name == "content") {
    Glib::ustring type;

    AttributeList::const_iterator it;
    for (it = attributes.begin(); it != attributes.end(); ++it) {
      if( (*it).name == "type") {
        type = (*it).value;
	break;
      }
    }
    if (type == "xhtml") {
      // Inline html. Ideally, we want to keep the contents of all the subtree as is.
      // TODO !
      pass_through = true;
      return;
    }
  } else {
    GenericParser::on_start_element(name, attributes);
  }

}

void
AtomParser::on_end_element(const Glib::ustring& name) {
  
  if (name == "content") {
    pass_through = false;
  }

  GenericParser::on_end_element(name);
}

