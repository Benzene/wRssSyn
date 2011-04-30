#include "tumblrparser.h"
#include <glibmm/convert.h> // For Glib::ConvertError

#include <iostream>

using namespace std;

TumblrParser::TumblrParser() : xmlpp::SaxParser() {
    in_entry = false;
    header_image = false;
}

TumblrParser::TumblrParser(sqlite3 * db, string id) : xmlpp::SaxParser() {
    in_entry = false;
    this->db = db;
    this->id = id;
    header_image = false;
}

TumblrParser::~TumblrParser() {
    
}

void
TumblrParser::on_start_document() {

//  cout << "on_start_document()" << endl;
  
}

void
TumblrParser::on_end_document() {
  
//  cout << "on_end_document()" << endl;
  
  try {
//  std::cout << "Feed Properties : " << endl;
//  std::cout << "Title : " << GlobTitle << endl;
//  std::cout << "Url : " << GlobUrl << endl;
//  std::cout << "Description : " << GlobDescr << endl;

  int retcode = 0;
  
  string query("INSERT OR IGNORE INTO sources VALUES (?,?,?,?,?,?,?)");
  sqlite3_stmt * sq_stmt;
  retcode = sqlite3_prepare_v2(db, query.c_str(), -1, &sq_stmt, NULL);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_prepare_v2 failed ! Retcode : " << retcode << endl;
  }
  sqlite3_bind_text(sq_stmt,1,id.c_str(),-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(1) failed ! Retcode : " << retcode << endl;
  }
  sqlite3_bind_text(sq_stmt,2,"Tumblr dashboard",-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(2) failed ! Retcode : " << retcode << endl;
  }
  sqlite3_bind_text(sq_stmt,3,"http://www.tumblr.com/dashboard",-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(3) failed ! Retcode : " << retcode << endl;
  }
  sqlite3_bind_text(sq_stmt,4,"Tumblr dashboard",-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(4) failed ! Retcode : " << retcode << endl;
  }
  sqlite3_bind_text(sq_stmt,5,"",-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(4) failed ! Retcode : " << retcode << endl;
  }
  sqlite3_bind_text(sq_stmt,6,"",-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(4) failed ! Retcode : " << retcode << endl;
  }
  sqlite3_bind_text(sq_stmt,7,"",-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(4) failed ! Retcode : " << retcode << endl;
  }
  sqlite3_step(sq_stmt);
  sqlite3_finalize(sq_stmt);

  // TODO: Check that this doesn't do weird things
  // the feed properties aren't added while the content is still parsed
  // => clutters the database, and these entries might appear for different
  // feeds
  } catch (Glib::ConvertError) {
    std::cout << "Parsing error. Could not parse the feed" << endl;
  }

}

void
TumblrParser::on_start_element(const Glib::ustring& name,
			    const AttributeList& attributes) {  

  CurrentTag = name;
  
  if (name == "post") {
    in_entry = true;
    CurrentEntry = new TumblrEntry();
    for(AttributeList::const_iterator it = attributes.begin(); it != attributes.end(); it++) {
      if (it->name == "id") {
        CurrentEntry->id = it->value;
      } else if (it->name == "url") {
        CurrentEntry->link = it->value;
	CurrentEntry->title = it->value;
      } else if (it->name == "type") {
        CurrentEntry->type = it->value;
      } else if (it->name == "date") {
        CurrentEntry->date = it->value;
      } else if (it->name == "tumblelog") {
        CurrentEntry->poster_id = it->value;
      }
//      cout << "Attribute: " + it->name + "(" + it->value + ")" << endl;
    }
  }
}

void
TumblrParser::on_end_element(const Glib::ustring& name) {
  
  CurrentTag = "";
  
  if (name == "post") {
    
    CurrentEntry->write_to_db(db, id);
    delete CurrentEntry;
    
    in_entry = false;
  }
}

void
TumblrParser::on_characters(const Glib::ustring& text) {

  if (in_entry) {
    if (CurrentTag == "photo-caption") {
      CurrentEntry->photo_caption += text;
    } else if (CurrentTag == "photo-url") {
      if (CurrentEntry->photo_url == "") {
        CurrentEntry->photo_url = text;
      }
    } else if (CurrentTag == "regular-title") {
      CurrentEntry->title = text;
    } else if (CurrentTag == "regular-body") {
      CurrentEntry->description += text;
    } else if (CurrentTag == "link-text") {
      CurrentEntry->description += text;
    } else if (CurrentTag == "link-url") {
      CurrentEntry->content_link += text;
    } else if (CurrentTag == "quote-text") {
      CurrentEntry->description += text;
    } else if (CurrentTag == "quote-source") {
      CurrentEntry->quote_source += text;
    } else if (CurrentTag == "question") {
      CurrentEntry->question += text;
    } else if (CurrentTag == "answer") {
      CurrentEntry->answer += text;
    }  
  }
    
}

void
TumblrParser::on_comment(const Glib::ustring& text) {

}

void
TumblrParser::on_warning(const Glib::ustring& text) {
  std::cout << "Warning : " << text << endl;
}

void
TumblrParser::on_error(const Glib::ustring& text) {
  std::cout << "Error : " << text << endl;
}

void
TumblrParser::on_fatal_error(const Glib::ustring& text) {
  std::cout << "Fatal error : " << text << endl;
}

