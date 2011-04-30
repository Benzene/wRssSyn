#include <iostream>
#include <cstdlib>
#include "tumblr_entry.h"

#include "time_helpers.h"


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

void
TumblrEntry::write_to_db(sqlite3 * db, std::string website_id) {
  int retcode = 0;
  
  string query("INSERT OR IGNORE INTO posts VALUES (?,?,?,?,?,?,0)");
  sqlite3_stmt * sq_stmt;
  retcode = sqlite3_prepare_v2(db, query.c_str(), -1, &sq_stmt, NULL);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_prepare_v2 failed ! Retcode : " << retcode << endl;
  }
  
  retcode = sqlite3_bind_text(sq_stmt,1,website_id.c_str(),-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(1) failed ! Retcode : " << retcode << endl;
  }
  
  if (id == "") {
    retcode = sqlite3_bind_text(sq_stmt,2,link.c_str(),-1,SQLITE_STATIC);
  } else {
    retcode = sqlite3_bind_text(sq_stmt,2,id.c_str(),-1,SQLITE_STATIC);
  }
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(2) failed ! Retcode : " << retcode << endl;
  }

  retcode = sqlite3_bind_text(sq_stmt,3,title.c_str(),-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(3) failed ! Retcode : " << retcode << endl;
  }
  
  retcode = sqlite3_bind_text(sq_stmt,4,link.c_str(),-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(4) failed ! Retcode : " << retcode << endl;
  }

  time_t ret = TimeHelpers::parseXMLtime(date);
  
  retcode = sqlite3_bind_int(sq_stmt,5,ret);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(5) failed ! Retcode : " << retcode << endl;
  }  
  
  Glib::ustring descr = build_content();
  retcode = sqlite3_bind_text(sq_stmt,6,descr.c_str(),-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(6) failed ! Retcode : " << retcode << endl;
  }
  
  sqlite3_step(sq_stmt);
  sqlite3_finalize(sq_stmt);
    
}

Glib::ustring
TumblrEntry::build_content() {
    Glib::ustring ret = "";
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

//    std::cout << "***" << endl;
//    std::cout << ret.c_str() << endl;
//    return "Blah";
    return ret;
}
