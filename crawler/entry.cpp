#include <iostream>
#include "entry.h"

using namespace std;

Entry::Entry() {
  
}

Entry::Entry(string id, string title, string link, time_t date, string description) :
  id(id),
  title(title),
  link(link),
  date(asctime(localtime(&date))),
  summary(""),
  description(description)
{
}

Entry::~Entry() {
  
}

void
Entry::print() {
  cout << "Entry " << id << endl;
  cout << "Title : " << title << endl;
  cout << "Url : " << link << endl;
  cout << "Date : " << date << endl;
  cout << "Summary : " << summary << endl;
  cout << "Description : " << description << endl;
}

void
Entry::print_title() {
  cout << "Title : " << title << endl;
}

