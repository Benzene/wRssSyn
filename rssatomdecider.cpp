#include "rssatomdecider.h"
#include <glibmm/convert.h> // For Glib::ConvertError

#include <iostream>

using namespace std;

RssAtomDecider::RssAtomDecider() : xmlpp::SaxParser() {
  feed_type = -1;

}

RssAtomDecider::~RssAtomDecider() {
    
}

void
RssAtomDecider::on_start_document() {
  
}

void
RssAtomDecider::on_end_document() {

}

void
RssAtomDecider::on_start_element(const Glib::ustring& name,
			    const AttributeList& attributes) {  

  if (name == "rss") {
    feed_type = 0;
  } else if (name == "feed") {
    feed_type = 1;
  }  
}

void
RssAtomDecider::on_end_element(const Glib::ustring& name) {
  
}

void
RssAtomDecider::on_characters(const Glib::ustring& text) {

}

void
RssAtomDecider::on_comment(const Glib::ustring& text) {
//  std::cout << "Comments : " << text << endl;
}

void
RssAtomDecider::on_warning(const Glib::ustring& text) {
  try {
    std::cerr << "Rss/Atom decider warning : " << text; // << endl;
  } catch (Glib::ConvertError e) {
    std::cerr << "Rss/Atom decider error on warning : " << e.what() << std::endl;
  }
}

void
RssAtomDecider::on_error(const Glib::ustring& text) {
  try {	
    std::cerr << "Rss/Atom decider error : " << text; // << endl;
  } catch (Glib::ConvertError e) {
    std::cerr << "Rss/Atom decider error on error : " << e.what() << std::endl;
  }
}

void
RssAtomDecider::on_fatal_error(const Glib::ustring& text) {
  try {
    std::cerr << "Rss/Atom decider fatal error : " << text; // << endl;
  } catch (Glib::ConvertError e) {
    std::cerr << "Rss/Atom decider error on fatal error : " << e.what() << std::endl;
  }
}

bool
RssAtomDecider::is_atom() {
  if (feed_type == 1)
    return true;
  else
    return false;
}    

bool
RssAtomDecider::is_rss2() {
  if (feed_type == 0)
    return true;
  else
    return false;
}    

bool
RssAtomDecider::is_unknown() {
  if (feed_type == -1)
    return true;
  else
    return false;
}    

