#include <cstdio>
#include <iostream>
#include <fstream>
#include <ctime>
#include <curl/curl.h>
#include <sqlite3.h>
#include "auth.h"
#include "rss2parser.h"
#include "atomparser.h"
#include "rssatomdecider.h"
#include "tumblr.h"
#include "tumblrparser.h"

using namespace std;

static char errorBuffer[CURL_ERROR_SIZE];

sqlite3 * init_database();
int update_feeds();
int update_feed(sqlite3 * db, string url, string id, string etag, string last_modified);
int get_feed(string id);
int list_feeds();
int add_feed(string name, string url);

void print_usage();

struct stored_headers {
	string * etag;
	string * lastmodified;
};
size_t parse_header(void * ptr, size_t size, size_t nmemb, void * userdata);

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
      if (argc < 4) {
	print_usage();
	return 1;
      } else {
	string name(argv[2]);
	string url(argv[3]);
	return add_feed(name,url);
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
    cerr << "  add [name] [url]	adds an rss url to the list" << endl; 
}

int
update_feeds() {
  
  ifstream fin("feedList.ini");
  
  /* Initialise database connection */
  sqlite3 * db = init_database();
  
  /*
   * For each url in feedList.ini :
   *  - Check if feed url is already in the database. If not, fetch and add.
   */
  cout << "Reading config file...";
  cout.flush();
  while (true) {
    std::string url;
    std::string id;
    fin >> url;
    fin >> id;
    if (fin.eof()) {
      break;
    }

    sqlite3_stmt * sq_stmt;
    int retcode = 0;
    string query("INSERT OR IGNORE INTO sources (website_id, feed_url, user, etag, lastmodified) VALUES (?,?,?,'','')");
    retcode = sqlite3_prepare_v2(db, query.c_str(), -1, &sq_stmt, NULL);
    if (retcode != SQLITE_OK) {
	    cerr << "Database connection failed at config file parsing ! (" << retcode << ")" << endl;
    }

    retcode = sqlite3_bind_text(sq_stmt, 1, id.c_str(), -1, SQLITE_STATIC);
    if (retcode != SQLITE_OK) {
	    cerr << "BindText failed at config file parsing ! (" << retcode << ")" << endl;
    }
    retcode = sqlite3_bind_text(sq_stmt, 2, url.c_str(), -1, SQLITE_STATIC);
    if (retcode != SQLITE_OK) {
	    cerr << "BindText failed at config file parsing ! (" << retcode << ")" << endl;
    }
    retcode = sqlite3_bind_text(sq_stmt, 3, glob_login.c_str(), -1, SQLITE_STATIC);
    if (retcode != SQLITE_OK) {
	    cerr << "BindText failed at config file parsing ! (" << retcode << ")" << endl;
    }
    sqlite3_step(sq_stmt);

  }
  cout << "done." << endl;

  /*
   * Next, update the already existing feeds.
   */
  int retcode = 0;
  sqlite3_stmt * sq_stmt;
  string query("SELECT website_id, feed_url, etag, lastmodified FROM sources");
  retcode = sqlite3_prepare_v2(db, query.c_str(), -1, &sq_stmt, NULL);
  if (retcode != SQLITE_OK) {
    cerr << "Failed to retrieve existing feeds ! (" << retcode << ")" << endl;
  }
  while( sqlite3_step(sq_stmt) == SQLITE_ROW) {
    update_feed(db, (char *)sqlite3_column_text(sq_stmt,1), (char *)sqlite3_column_text(sq_stmt,0), (char *)sqlite3_column_text(sq_stmt,2), (char *)sqlite3_column_text(sq_stmt,3));
  }

  /*
   * Finally, parse the special purpose feeds.
   * Only tumblr is in there, for now.
   */
  update_tumblr_feeds(db);
  TumblrParser parser(db);
  parser.set_substitute_entities(true);
  parser.parse_file("tumblrdashboard.auto.xml");
  
  return 0;
  
}

int update_feed(sqlite3 * db, string url, string id, string etag, string lastmodified) {

  /* Empty urls are used for custom feeds. We can silently skip them. */	
  if (url.compare("") == 0) {
	  return 4;
  }

  FILE * target = fopen ( (id+".auto.xml").c_str(), "w");
    
  CURL *curl;
  CURLcode result;
    
  curl = curl_easy_init();
    
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HEADER, 0);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, target);
      
    /* Set the custom headers */
    struct curl_slist * slist = NULL;
    string ifmodifheader = "If-Modified-Since: ";
    ifmodifheader.append(lastmodified);
    string ifetagheader = "If-None-Match: ";
    ifetagheader.append(etag);
    slist = curl_slist_append(slist, ifmodifheader.c_str());
    slist = curl_slist_append(slist, ifetagheader.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);

    /* Read the custom headers */
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &parse_header);
    struct stored_headers * headersRecipient = new struct stored_headers;
    headersRecipient->etag = new string("");
    headersRecipient->lastmodified = new string("");
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, headersRecipient);

    result = curl_easy_perform(curl);
    if(result != 0) {
      cout << "Connection error for " << url << " : " << errorBuffer << endl; 
      return 3;
    }

    long HttpResponseCode = 0;
    result = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &HttpResponseCode);
//    cout << "HTTP : " << HttpResponseCode << endl;
    
    if (HttpResponseCode == 304) {
//	    cout << "Not modified. Skipping." << endl;
	    return 1;
    } else if (HttpResponseCode != 200) {
//	    cout << "Retrieving failed. Skipping." << endl;
	    return 3;
    }
      
    curl_slist_free_all(slist);
    curl_easy_cleanup(curl);
    
    fclose(target);
    
    cout << "Updating " << id << " (" << url << ")" << endl;

    RssAtomDecider preparser;
    // Convert html entities to normal characters
    preparser.set_substitute_entities(true);
    preparser.parse_file(id + ".auto.xml");
    if (preparser.is_rss2()) {
      cout << "(Rss feed)";
      RssParser parser(db, id);
      parser.set_substitute_entities(true);
      parser.parse_file(id + ".auto.xml");
    } else if (preparser.is_atom()) {
      cout << "(Atom feed)";
      AtomParser parser(db,id);
      parser.set_substitute_entities(true);
      parser.parse_file(id + ".auto.xml");
    }

    /*
     * Finally, update etag and lastmodified.
     * It could be done earlier, but we would have no assurance that everything went right, and that we indeed updated. Better to do it that way.
     */
    sqlite3_stmt * sq_stmt;
    int retcode = 0;
    string query("UPDATE sources SET etag=?, lastmodified=? WHERE website_id=? LIMIT 1");
    retcode = sqlite3_prepare_v2(db, query.c_str(), -1, &sq_stmt, NULL);
    if (retcode != SQLITE_OK) {
	    cerr << "Unable to update Etag/LastModified ! (" << retcode << ")" << endl;
    }

    retcode = sqlite3_bind_text(sq_stmt, 1, headersRecipient->etag->c_str(), -1, SQLITE_STATIC);
    if (retcode != SQLITE_OK) {
	    cerr << "BindText failed at Etag update ! (" << retcode << ")" << endl;
    }

    retcode = sqlite3_bind_text(sq_stmt, 2, headersRecipient->lastmodified->c_str(), -1, SQLITE_STATIC);
    if (retcode != SQLITE_OK) {
	    cerr << "BindText failed at Etag update ! (" << retcode << ")" << endl;
    }

    retcode = sqlite3_bind_text(sq_stmt, 3, id.c_str(), -1, SQLITE_STATIC);
    if (retcode != SQLITE_OK) {
	    cerr << "BindText failed at Etag update ! (" << retcode << ")" << endl;
    }
    sqlite3_step(sq_stmt);

    delete headersRecipient->etag;
    delete headersRecipient->lastmodified;

    cout << " done" << endl;
  }

  return 0;
}

size_t parse_header(void * ptr, size_t size, size_t nmemb, void * userdata) {
	string str((char *)ptr, size *nmemb);
//	cout << "Header found :" << str << endl;

	/* We split the header into : The title, the content */

	size_t f = str.find(':');
	if (f == string::npos) {
//		cerr << "Malformed header : " << str << endl;
	} else {
		size_t endLine = str.find_first_of("\r\n", f);
		if (endLine == string::npos) {
			cerr << "No newline ? *Very malformed header : " << str << endl;
		} else {
			string title = str.substr(0, f);
			string content = str.substr(f+2, endLine-f-2);

			if (title.compare("ETag") == 0 || title.compare("Last-Modified") == 0) {
//				cout << "Relevant header : " << title << " -> " << content << endl;
			}
			if (title.compare("ETag") == 0) {
				((struct stored_headers *)userdata)->etag->append(content);
			} else if (title.compare("Last-Modified") == 0) {
				((struct stored_headers *)userdata)->lastmodified->append(content);
			}
		}
	}
	
	return size * nmemb;
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

int list_feeds() {
  
  sqlite3 * db = init_database();
  sqlite3_stmt * sq_stmt;
  int retcode = 0;
  
  string query("SELECT * FROM sources");
  
  retcode = sqlite3_prepare_v2(db, query.c_str(), -1, &sq_stmt, NULL);

  while ( sqlite3_step(sq_stmt) == SQLITE_ROW) {
    cout << " * " << (char *)sqlite3_column_text(sq_stmt,0);
    cout << ": " << (char *)sqlite3_column_text(sq_stmt,1);
    cout << " (" << (char *)sqlite3_column_text(sq_stmt,2);
    cout << ")" << endl;
  }
    
  sqlite3_finalize(sq_stmt);
  
  return 0;
}

int add_feed(string name, string url) {

  ofstream fout("feedList.ini", ios_base::app);
  
  fout << url << " " << name << endl;
  fout.close();
  
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
  
  string query("CREATE TABLE IF NOT EXISTS posts (website_id TEXT, id TEXT, title TEXT, link TEXT, date INTEGER, description TEXT, read INTEGER, user TEXT, PRIMARY KEY (website_id, id))");
  sqlite3_stmt * sq_stmt;
  sqlite3_prepare_v2(db, query.c_str(), -1, &sq_stmt, NULL);
  sqlite3_step(sq_stmt);
  sqlite3_finalize(sq_stmt);
  
  string query2("CREATE TABLE IF NOT EXISTS sources (website_id TEXT PRIMARY KEY, feed_url TEXT, title TEXT, url TEXT, descr TEXT, imgtitle TEXT, imgurl TEXT, imglink TEXT, user TEXT, etag TEXT, lastmodified TEXT)");
  sqlite3_prepare_v2(db, query2.c_str(), -1, &sq_stmt, NULL);
  sqlite3_step(sq_stmt);
  sqlite3_finalize(sq_stmt);
  
  return db;
}
