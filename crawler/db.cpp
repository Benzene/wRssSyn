#include "db.h"
#include "time_helpers.h"
#include <iostream>
#include <cstdlib>

DBValue::DBValue() {
  type = DBVAL_EMPTY;
}

DBValue::DBValue(int v) {
  type = DBVAL_INT;
  this->v = v;
}

DBValue::DBValue(std::string str) {
  type = DBVAL_STR;
  this->str = str;
}

DBValue::DBValue(const char * str) {
  type = DBVAL_STR;
  this->str = std::string(str);
}

int DBValue::getType() {
  return type;
}

int DBValue::getInt() {
  if(type == DBVAL_INT) {
    return v;
  } else {
    std::cerr << "Int access to a non-int DBValue. Something is wrong." << std::endl;
    exit(-1);
  }
}

std::string DBValue::getStr() {
  if(type == DBVAL_STR) {
    return str;
  } else {
    std::cerr << "Str access to a non-str DBValue. Something is wrong." << std::endl;
    exit(-1);
  }
}

AbstractDB::AbstractDB() {

}

AbstractDB::~AbstractDB() {

}

int
AbstractDB::create_feed(const feed_type& feed_type, const std::string& feed_url, const std::string& user) {

  std::string blank("");

  struct feed f;
  f.uid = -1;
  f.type = feed_type;
  f.feed_url = feed_url;
  f.url = "";
  f.descr = "";
  f.title = "";
  f.etag = "";
  f.lastmodified = "";

  return create_feed_full(f, user);
}

void
AbstractDB::insert_entry(int id, Entry &entry) {
  std::string * uid = NULL;
  if (entry.id == "") {
    uid = &(entry.link);
  } else {
    uid = &(entry.id);
  }

  std::string * descr = NULL;
  if (entry.description == "") {
    descr = &(entry.summary);
  } else {
    descr = &(entry.description);
  }

  time_t ret = TimeHelpers::parseXMLtime(entry.date);

  insert_entry(id, *uid, entry.title, entry.link, ret, *descr);
}

/*
void
AbstractDB::insert_entry(std::string &website_id, TumblrEntry &entry) {
  entry.description = entry.build_content();

  insert_entry(website_id, (Entry&)entry);
}
*/
