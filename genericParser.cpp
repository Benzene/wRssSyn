#include "genericParser.h"

#include <glibmm/convert.h>
#include <iostream>

GenericParser::GenericParser() : 
  xmlpp::SaxParser(),
  in_entry(false),
  header_image(false) 
{
}

GenericParser::GenericParser(AbstractDB * db, std::string id) : 
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
    
    db->update_metadata_feed(id, GlobTitle, GlobUrl, GlobDescr, ImgTitle, ImgUrl, ImgLink);

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
    db->insert_entry(id, *CurrentEntry);
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
}

void
GenericParser::on_warning(const Glib::ustring& text) {
  std::cerr << getParserName() << " warning : ";
  try {
    std::cerr << text;
  } catch(Glib::ConvertError e) {
    std::cerr << "ConvertError" << std::endl;
  }
}

void
GenericParser::on_error(const Glib::ustring& text) {
  std::cerr << getParserName() << " error : ";
  try {
    std::cerr << text;
  } catch(Glib::ConvertError e) {
    std::cerr << "ConverError" << std::endl;
  }
}

void
GenericParser::on_fatal_error(const Glib::ustring& text) {
  std::cerr << getParserName() << " fatal error : " << text;
  try {
    std::cerr << text;
  } catch (Glib::ConvertError e) {
    std::cerr << "ConvertError" << std::endl;
  }
}

