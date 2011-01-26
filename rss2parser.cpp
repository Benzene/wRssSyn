#include "rss2parser.h"
#include <glibmm/convert.h> // For Glib::ConvertError

#include <iostream>

using namespace std;

RssParser::RssParser() : xmlpp::SaxParser() {
    in_entry = false;
}

RssParser::~RssParser() {
  
}

void
RssParser::on_start_document() {
  std::cout << "on_start_document()" << endl;
}

void
RssParser::on_end_document() {
  std::cout << "on_end_document()" << endl; 
  std::cout << "Feed Properties : " << endl;
  std::cout << "Title : " << GlobTitle << endl;
  std::cout << "Url : " << GlobUrl << endl;
  std::cout << "Description : " << GlobDescr << endl;
}

void
RssParser::on_start_element(const Glib::ustring& name,
			    const AttributeList& attributes) {

  /* 
  std::cout << "node name=" << name << endl;
  for (xmlpp::SaxParser::AttributeList::const_iterator iter = attributes.begin();
       iter != attributes.end();
       ++iter) {
    std::cout << "  Attribute name=" << iter->name << std::endl;
    std::cout << "    , value=" << iter->value << std::endl;
  }
  */
  
  CurrentTag = name;
  
  if (name == "item") {
    in_entry = true;
    CurrentEntry = new Entry();
  }
}

void
RssParser::on_end_element(const Glib::ustring& name) {
  /*
  std::cout << "on_end_element(" << name << ")" << endl;
  */
  if (name == "item") {
    
    CurrentEntry->print();
    delete CurrentEntry;
    
    in_entry = false;
  }
}

void
RssParser::on_characters(const Glib::ustring& text) {
  /*
  std::cout << "Text : " << text << endl;
  */
  
  if (CurrentTag == "title") {
    if (!in_entry) {
      GlobTitle += text;
    } else {
      CurrentEntry->title += text;
    }
  } else if (CurrentTag == "link") {
    if (!in_entry) {
      GlobUrl += text;
    } else {
      CurrentEntry->link += text;
    }
  } else if (CurrentTag == "description") {
    if (!in_entry) {
      GlobDescr += text;
    } else {
      CurrentEntry->description += text;
    }
  } else if (CurrentTag == "pubDate") {
    if (in_entry) {
      CurrentEntry->date += text;
    }
  } else if (CurrentTag == "guid") {
    if (in_entry) {
      CurrentEntry->id += text;
    }
  }
    
}

void
RssParser::on_comment(const Glib::ustring& text) {
  std::cout << "Comments : " << text << endl;
}

void
RssParser::on_warning(const Glib::ustring& text) {
  std::cout << "Warning : " << text << endl;
}

void
RssParser::on_error(const Glib::ustring& text) {
  std::cout << "Error : " << text << endl;
}

void
RssParser::on_fatal_error(const Glib::ustring& text) {
  std::cout << "Fatal error : " << text << endl;
}

