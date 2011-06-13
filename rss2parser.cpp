#include "rss2parser.h"
#include <glibmm/convert.h> // For Glib::ConvertError

#include <iostream>
#include "auth.h"

using namespace std;

RssParser::RssParser() : xmlpp::SaxParser() {
    in_entry = false;
    header_image = false;
}

RssParser::RssParser(sqlite3 * db, string id) : xmlpp::SaxParser() {
    in_entry = false;
    this->db = db;
    this->id = id;
    header_image = false;
}

RssParser::~RssParser() {
    
}

void
RssParser::on_start_document() {

//  cout << "on_start_document()" << endl;
  
}

void
RssParser::on_end_document() {
  
//  cout << "on_end_document()" << endl;
  
  try {
//  std::cout << "Feed Properties : " << endl;
//  std::cout << "Title : " << GlobTitle << endl;
//  std::cout << "Url : " << GlobUrl << endl;
//  std::cout << "Description : " << GlobDescr << endl;

  int retcode = 0;
  
  string query("INSERT OR IGNORE INTO sources VALUES (?,?,?,?,?,?,?,?)");
  sqlite3_stmt * sq_stmt;
  retcode = sqlite3_prepare_v2(db, query.c_str(), -1, &sq_stmt, NULL);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_prepare_v2 failed ! Retcode : " << retcode << endl;
  }
  sqlite3_bind_text(sq_stmt,1,id.c_str(),-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(1) failed ! Retcode : " << retcode << endl;
  }
  sqlite3_bind_text(sq_stmt,2,GlobTitle.c_str(),-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(2) failed ! Retcode : " << retcode << endl;
  }
  sqlite3_bind_text(sq_stmt,3,GlobUrl.c_str(),-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(3) failed ! Retcode : " << retcode << endl;
  }
  sqlite3_bind_text(sq_stmt,4,GlobDescr.c_str(),-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(4) failed ! Retcode : " << retcode << endl;
  }
  sqlite3_bind_text(sq_stmt,5,ImgTitle.c_str(),-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(4) failed ! Retcode : " << retcode << endl;
  }
  sqlite3_bind_text(sq_stmt,6,ImgUrl.c_str(),-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(4) failed ! Retcode : " << retcode << endl;
  }
  sqlite3_bind_text(sq_stmt,7,ImgLink.c_str(),-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(4) failed ! Retcode : " << retcode << endl;
  }
  sqlite3_bind_text(sq_stmt,8,glob_login.c_str(),-1,SQLITE_STATIC);
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
RssParser::on_start_element(const Glib::ustring& name,
			    const AttributeList& attributes) {  

//  cout << "on_start_element(" << name << ")" << endl;
  
  CurrentTag = name;
  
  if (name == "item") {
    in_entry = true;
    CurrentEntry = new Entry();
  } else if (!in_entry && name == "image") {
    header_image = true;
  }
}

void
RssParser::on_end_element(const Glib::ustring& name) {
  
//  cout << "on_end_element(" << name << ")" << endl;

  CurrentTag = "";
  
  if (name == "item") {
    
//    CurrentEntry->print_title();
    CurrentEntry->write_to_db(db, id);
    delete CurrentEntry;
    
    in_entry = false;
  } else if (!in_entry && name == "image") {
    header_image = false;
  }
}

void
RssParser::on_characters(const Glib::ustring& text) {

  try {
  
//  cout << "on_characters(" << text << ")" << endl;
  
  if (!in_entry && !header_image) {
    if (CurrentTag == "title") {
      GlobTitle += text;
    } else if (CurrentTag == "link") {
      GlobUrl += text;
    } else if (CurrentTag == "description") {
      GlobDescr += text;
    }
  }
  
  if (!in_entry && header_image) {
    if (CurrentTag == "title") {
      ImgTitle += text;
    } else if (CurrentTag == "url") {
      ImgUrl += text;
    } else if (CurrentTag == "link") {
      ImgLink +=text;
    }
  }
  
  if (in_entry) {
    if (CurrentTag == "title") {
      CurrentEntry->title += text;
    } else if (CurrentTag == "link") {
      CurrentEntry->link += text;
    } else if (CurrentTag == "description") {
      CurrentEntry->description += text;
    } else if (CurrentTag == "pubDate") {
      CurrentEntry->date += text;
    } else if (CurrentTag == "guid") {
      CurrentEntry->id += text;
    }
  }
    
  } catch (Glib::ConvertError) {
    std::cout << "Parsing error." << endl;
  }  
}

void
RssParser::on_cdata_block(const Glib::ustring& text) {
	on_characters(text);
}	

void
RssParser::on_comment(const Glib::ustring& text) {
//  std::cout << "Comments : " << text << endl;
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

