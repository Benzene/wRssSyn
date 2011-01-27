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

void
Entry::print_title() {
  cout << "Title : " << title << endl;
}

void
Entry::write_to_db(sqlite3 * db, std::string website_id) {
  int retcode = 0;
  
  string query("INSERT OR IGNORE INTO posts VALUES (?,?,?,?,?,?)");
  sqlite3_stmt * sq_stmt;
  retcode = sqlite3_prepare_v2(db, query.c_str(), -1, &sq_stmt, NULL);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_prepare_v2 failed ! Retcode : " << retcode << endl;
  }
  sqlite3_bind_text(sq_stmt,1,website_id.c_str(),-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(1) failed ! Retcode : " << retcode << endl;
  }
  sqlite3_bind_text(sq_stmt,2,id.c_str(),-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(2) failed ! Retcode : " << retcode << endl;
  }
  sqlite3_bind_text(sq_stmt,3,title.c_str(),-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(3) failed ! Retcode : " << retcode << endl;
  }
  sqlite3_bind_text(sq_stmt,4,link.c_str(),-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(4) failed ! Retcode : " << retcode << endl;
  }
  sqlite3_bind_text(sq_stmt,5,date.c_str(),-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(5) failed ! Retcode : " << retcode << endl;
  }
  sqlite3_bind_text(sq_stmt,6,description.c_str(),-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(6) failed ! Retcode : " << retcode << endl;
  }
  sqlite3_step(sq_stmt);
  sqlite3_finalize(sq_stmt);
  
  
}
