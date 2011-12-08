#include <iostream>
#include <cstdlib>
#include "entry.h"

#include "auth.h"
#include "time_helpers.h"


using namespace std;

Entry::Entry() {
  
}

Entry::Entry(string id, string title, string link, time_t date, string description) {
  this->id = id;
  this->title = title;
  this->link = link;
  string strdate(asctime(localtime(&date)));
  this->date = strdate;
  this->description = description;
}

Entry::~Entry() {
  
}

void
Entry::print() {
  cout << "Entry " << id << endl;
  cout << "Title : " << title << endl;
  cout << "Url : " << link << endl;
  cout << "Date : " << date << endl;
  cout << "Description : " << description << endl;
}

void
Entry::print_title() {
  cout << "Title : " << title << endl;
}

void
Entry::write_to_db(AbstractDB * db, std::string website_id) {

  std::string * uid = NULL;
  if (id == "") {
    uid = &link;
  } else {
    uid = &id;
  }

//  cout << "Parsing date : " << date << endl;
  time_t ret = TimeHelpers::parseXMLtime(date);
//  cout << "parseXMLtime() returned : " << ret << endl;
//
  db->insert_entry(website_id, *uid, title, link, ret, description, glob_login);
  
}
