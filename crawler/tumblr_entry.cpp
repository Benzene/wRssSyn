#include <iostream>
#include "tumblr_entry.h"

using namespace std;

TumblrEntry::TumblrEntry() {
  
}

TumblrEntry::~TumblrEntry() {
  
}

void
TumblrEntry::print() {
  cout << "Entry " << id << endl;
  cout << "Title : " << title << endl;
  cout << "Url : " << link << endl;
  cout << "Date : " << date << endl;
  cout << "Description : " << description << endl;
}

std::string
TumblrEntry::build_content() {
    std::string ret = "";
    if (type == "photo") {
      ret += "<div style=\"float:left;\">";
      ret += "<img src=\"";
      ret += photo_url;
      ret += "\" /></div>";
      ret += photo_caption;
    } else if (type == "regular") {
      ret += description;
    } else if (type == "link") {
      ret += "<a href=\"";
      ret += content_link;
      ret += "\">Lien</a><br />";
      ret += description;
    } else if (type == "quote") {
      ret += description;
      ret += "<br />(Source : ";
      ret += quote_source;
      ret += ")";
    } else if (type == "conversation") {
      ret += "Conversation (skipped..)";
    } else if (type == "video") {
      ret += "Video (skipped)";
    } else if (type == "audio") {
      ret += "Audio (skipped)";
    } else if (type == "answer") {
      ret += "Question : ";
      ret += question;
      ret += "<br />Answer : ";
      ret += answer;
    }  

    return ret;
}
