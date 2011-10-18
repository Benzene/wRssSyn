#include "genericParser.h"

#include <glibmm/convert.h>
#include <iostream>

GenericParser::GenericParser() : 
  xmlpp::SaxParser(),
  in_entry(false),
  header_image(false) 
{
}

GenericParser::GenericParser(sqlite3 * db, std::string id) : 
  xmlpp::SaxParser(),
  in_entry(false),
  db(db),
  id(id),
  header_image(false)
{
}

GenericParser::~GenericParser() {

}

void
GenericParser::on_start_document() {
}

void
GenericParser::on_end_document() {

  try {
    
    int retcode = 0;

    std::string query("UPDATE sources SET title=?, url=?, descr=?, imgtitle=?, imgurl=?, imglink=? WHERE website_id=? LIMIT 1");
    sqlite3_stmt * sq_stmt;
  
    retcode = sqlite3_prepare_v2(db, query.c_str(), -1, &sq_stmt, NULL);
    if (retcode != SQLITE_OK) {
	    std::cerr << "sqlite3_prepare_v2 failed ! Retcode : " << retcode << std::endl;
    }
    sqlite3_bind_text(sq_stmt,1,GlobTitle.c_str(),-1,SQLITE_STATIC);
    if (retcode != SQLITE_OK) {
	    std::cerr << "sqlite3_bind_text(1) failed ! Retcode : " << retcode << std::endl;
    }
    sqlite3_bind_text(sq_stmt,2,GlobUrl.c_str(),-1,SQLITE_STATIC);
    if (retcode != SQLITE_OK) {
	    std::cerr << "sqlite3_bind_text(2) failed ! Retcode : " << retcode << std::endl;
    }
    sqlite3_bind_text(sq_stmt,3,GlobDescr.c_str(),-1,SQLITE_STATIC);
    if (retcode != SQLITE_OK) {
	    std::cerr << "sqlite3_bind_text(3) failed ! Retcode : " << retcode << std::endl;
    }
    sqlite3_bind_text(sq_stmt,4,ImgTitle.c_str(),-1,SQLITE_STATIC);
    if (retcode != SQLITE_OK) {
	    std::cerr << "sqlite3_bind_text(4) failed ! Retcode : " << retcode << std::endl;
    }
    sqlite3_bind_text(sq_stmt,5,ImgUrl.c_str(),-1,SQLITE_STATIC);
    if (retcode != SQLITE_OK) {
	    std::cerr << "sqlite3_bind_text(5) failed ! Retcode : " << retcode << std::endl;
    }
    sqlite3_bind_text(sq_stmt,6,ImgLink.c_str(),-1,SQLITE_STATIC);
    if (retcode != SQLITE_OK) {
	    std::cerr << "sqlite3_bind_text(6) failed ! Retcode : " << retcode << std::endl;
    }
    sqlite3_bind_text(sq_stmt,7,id.c_str(),-1,SQLITE_STATIC);
    if (retcode != SQLITE_OK) {
	    std::cerr << "sqlite3_bind_text(7) failed ! Retcode : " << retcode << std::endl;
    }
 
    sqlite3_step(sq_stmt);
    sqlite3_finalize(sq_stmt);

    // TODO: Check that this doesn't do weird things
    // the feed properties aren't added while the content is still parsed
    // => clutters the database, and these entries might appear for different
    // feeds
    } catch (Glib::ConvertError) {
      std::cerr << "Parsing error. Could not parse the feed" << std::endl;
    }

}

void
GenericParser::on_start_element(const Glib::ustring& name,
		const AttributeList& attributes) {

  CurrentTag = name;

  if (name == getItemLabel()) {
    in_entry = true;
    CurrentEntry = new Entry();
  } else if (!in_entry && name == "image") {
    header_image = true;
  }
}

void
GenericParser::on_end_element(const Glib::ustring& name) {

  CurrentTag = "";

  if (name == getItemLabel()) {
    CurrentEntry->write_to_db(db, id);
    delete CurrentEntry;

    in_entry = false;
  } else if (!in_entry && name == "image") {
    header_image = false;
  }
}

void
GenericParser::on_characters(const Glib::ustring& text) {
  	
  try {
  
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
    } else if (CurrentTag == getContentLabel()) {
      CurrentEntry->description += text;
    } else if (CurrentTag == getPubDateLabel()) {
      CurrentEntry->date += text;
    } else if (CurrentTag == getUidLabel()) {
      CurrentEntry->id += text;
    }
  }
    
  } catch (Glib::ConvertError) {
    std::cerr << "Parsing error." << std::endl;
  }  
}

void
GenericParser::on_cdata_block(const Glib::ustring& text) {
  on_characters(text);	
}

void
GenericParser::on_comment(const Glib::ustring& text) {
//  std::cout << "Comments : " << text << endl;
}

void
GenericParser::on_warning(const Glib::ustring& text) {
  std::cerr << getParserName() << " warning : " << text << std::endl;
}

void
GenericParser::on_error(const Glib::ustring& text) {
  std::cerr << getParserName() << " error : " << text << std::endl;
}

void
GenericParser::on_fatal_error(const Glib::ustring& text) {
  std::cerr << getParserName() << " fatal error : " << text << std::endl;
}

