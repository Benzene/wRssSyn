#include <iostream>
#include "entry.h"

using namespace std;

Entry::Entry() {
  
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
