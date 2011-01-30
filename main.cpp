#include <cstdio>
#include <iostream>
#include <fstream>
#include <ctime>
#include <curl/curl.h>
#include <sqlite3.h>
#include "rss2parser.h"

using namespace std;

static char errorBuffer[CURL_ERROR_SIZE];

sqlite3 * init_database();
int update_feeds();
int get_feed(string id);
int list_feeds() {}
int add_feed(string url) {}

void print_usage();


int
main (int argc, char* argv[]) {
  
  // Supposed to make the encoding right..
  std::locale::global(std::locale(""));
  
  if (argc < 2) {
    print_usage();
    return 1;
  } else {
    string cmd(argv[1]);
    if (cmd == "update") {
      return update_feeds();
    } else if ( cmd == "get") {
      if (argc < 3) {
	print_usage();
	return 1;
      } else {
	string id(argv[2]);
	return get_feed(id);
      }
    } else if ( cmd == "list") {
      return list_feeds();
    } else if ( cmd == "add") {
      if (argc < 3) {
	print_usage();
	return 1;
      } else {
	string url(argv[2]);
	return add_feed(url);
      }
    } else {
      print_usage();
      return 1;
    }
  }
      
  
  return 0;
}

void
print_usage() {
    cerr << "Usage : wRssSyn <command>" << endl;
    cerr << "With <command> is one of : " << endl;
    cerr << "  update 		updates the feeds" << endl;
    cerr << "  get [feed_id]	prints last entries for feed [feed_id]" << endl;
    cerr << "  list		prints the list of feeds currently registered" << endl;
    cerr << "  add [url]	adds an rss url to the list" << endl; 
}

int
update_feeds() {
  
  ifstream fin("feedList.ini");
  
  /* Initialise database connection */
  sqlite3 * db = init_database();
  
  /*
   * For each url in feedList.ini :
   *  - If file already exists : compute current file checksum.
   *  - Get the file, and replace the old one.
   *  - Compute new checksum. If unchanged, do nothing.
   *  - Read the file, update the feeds.
   */
  while (true) {
    std::string url;
    std::string id;
    fin >> url;
    fin >> id;
    if (fin.eof()) {
      break;
    }
    cout << id << " : " << url << endl;

    FILE * target = fopen ( (id+".auto.xml").c_str(), "w");
    
    CURL *curl;
    CURLcode result;
    
    curl = curl_easy_init();
    
    if(curl) {
      curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl, CURLOPT_HEADER, 0);
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, target);
      
      result = curl_easy_perform(curl);
      if(result != 0) {
	cout << "Connection error : " << errorBuffer << endl; 
	return 3;
      }
      
      curl_easy_cleanup(curl);
    }
    
    fclose(target);
    
    RssParser parser(db, id);
//    parser.set_substitute_entities(true); // ?
    parser.parse_file(id + ".auto.xml");

  }
  
  return 0;
  
}

int
get_feed(string id) {
  sqlite3 * db = init_database();
  sqlite3_stmt * sq_stmt;
  int retcode = 0;
  
  string query("SELECT * FROM posts WHERE website_id=? ORDER BY date DESC LIMIT 10");
  
  retcode = sqlite3_prepare_v2(db, query.c_str(), -1, &sq_stmt, NULL);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_prepare_v2 failed ! Retcode : " << retcode << endl;
  }
  
  retcode = sqlite3_bind_text(sq_stmt,1,id.c_str(),-1,SQLITE_STATIC);
  if (retcode != SQLITE_OK) {
    cout << "sqlite3_bind_text(1) failed ! Retcode : " << retcode << endl;
  }
  while ( sqlite3_step(sq_stmt) == SQLITE_ROW) {
    string id((char *)sqlite3_column_text(sq_stmt,1));
    string title((char *)sqlite3_column_text(sq_stmt,2));
    string link((char *)sqlite3_column_text(sq_stmt,3));
    time_t date = sqlite3_column_int(sq_stmt,4);
    string description((char *)sqlite3_column_text(sq_stmt,5));
    Entry e(id, title, link, date, description);
    
    e.print();
  }
    
  sqlite3_finalize(sq_stmt);
  
  return 0;
}



/*
 * Open the connection and create the tables if they don't already exist.
 */
sqlite3 * 
init_database() {
  sqlite3 * db;
  string dbName = "feeds.db";
  sqlite3_open_v2(dbName.c_str(), &db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE,NULL);
  
  string query("CREATE TABLE IF NOT EXISTS posts (website_id TEXT, id TEXT, title TEXT, link TEXT, date INTEGER, description TEXT, PRIMARY KEY (website_id, id))");
  sqlite3_stmt * sq_stmt;
  sqlite3_prepare_v2(db, query.c_str(), -1, &sq_stmt, NULL);
  sqlite3_step(sq_stmt);
  sqlite3_finalize(sq_stmt);
  
  string query2("CREATE TABLE IF NOT EXISTS sources (website_id TEXT PRIMARY KEY, title TEXT, url TEXT, descr TEXT)");
  sqlite3_prepare_v2(db, query2.c_str(), -1, &sq_stmt, NULL);
  sqlite3_step(sq_stmt);
  sqlite3_finalize(sq_stmt);
  
  return db;
}
