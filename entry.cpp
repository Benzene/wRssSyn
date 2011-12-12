#include <iostream>
#include "entry.h"

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

