#include <cstdio>
#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <sqlite3.h>
#include "rss2parser.h"

using namespace std;

static char errorBuffer[CURL_ERROR_SIZE];

sqlite3 * init_database();

int main (int argc, char* argv[]) {
  
  // Supposed to make the encoding right..
  std::locale::global(std::locale(""));
  
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

/*
 * Open the connection and create the tables if they don't already exist.
 */
sqlite3 * 
init_database() {
  sqlite3 * db;
  string dbName = "feeds.db";
  sqlite3_open_v2(dbName.c_str(), &db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE,NULL);
  
  string query("CREATE TABLE IF NOT EXISTS posts (website_id TEXT, id TEXT, title TEXT, link TEXT, date TEXT, description TEXT, PRIMARY KEY (website_id, id))");
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
