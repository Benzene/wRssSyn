#include "db.h"
#include "auth.h"
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

void
AbstractDB::create_feed(std::string &website_id, std::string &feed_url, std::string &user) {

  std::string blank("");

  create_feed_full(website_id, feed_url, blank, blank, blank, blank, blank, blank, user, blank, blank);
}

void
AbstractDB::insert_entry(std::string &website_id, Entry &entry) {
  std::string &uid = entry.id;
  if (entry.id == "") {
    uid = entry.link;
  } 

  time_t ret = TimeHelpers::parseXMLtime(entry.date);

  insert_entry(website_id, uid, entry.title, entry.link, ret, entry.description, glob_login);
}

void
AbstractDB::insert_entry(std::string &website_id, TumblrEntry &entry) {
  entry.description = entry.build_content();

  insert_entry(website_id, (Entry&)entry);
}
