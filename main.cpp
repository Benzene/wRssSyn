#include <cstdio>
#include <iostream>
#include <fstream>
#include <ctime>
#include <curl/curl.h>
/* #include <sqlite3.h> */
#include "db.h"
#include "sqlite3db.h"
#include "postgresdb.h"
#include "feed.h"
#include "auth.h"
#include "rss2parser.h"
#include "atomparser.h"
#include "rssatomdecider.h"
#include "tumblr.h"
#include "tumblrparser.h"

using namespace std;

static char errorBuffer[CURL_ERROR_SIZE];

AbstractDB * init_database();
int update_feeds();
int update_feed(AbstractDB * db, struct feed * f);
//int update_feed(sqlite3 * db, string url, string id, string etag, string last_modified);
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
  AbstractDB * db = init_database();
  
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

    db->create_feed(id, url, glob_login);
  }
  cout << "done." << endl;

  /*
   * Next, update the already existing feeds.
   */
  std::list<struct feed *> * f = db->get_feeds();
  std::list<struct feed *>::iterator it;
  for (it = f->begin(); it != f->end(); ++it) {
    update_feed(db, (*it));
  }


  // TODO: free the memory of the feeds ?

  /*
   * Finally, parse the special purpose feeds.
   * Only tumblr is in there, for now.
   */
  update_tumblr_feeds(db);
  TumblrParser parser(db);
  parser.set_substitute_entities(true);
  try {
    parser.parse_file("tumblrdashboard.auto.xml");
  } catch(xmlpp::parse_error e) {
    std::cerr << "Parsing error: " << e.what() << std::endl;
  }
  
  return 0;
  
}

//int update_feed(sqlite3 * db, string url, string id, string etag, string lastmodified) {
int update_feed(AbstractDB * db, struct feed * f) {

  std::string id = *(f->id);
  std::string url = *(f->feed_url);
  std::string etag = *(f->etag);
  std::string lastmodified = *(f->lastmodified);

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
    try {
      preparser.parse_file(id + ".auto.xml");
    } catch(xmlpp::parse_error e) {
      std::cerr << "Preparsing error: " << e.what() << std::endl;
      return 6;
    }
    if (preparser.is_rss2()) {
      cout << "(Rss feed)";
      RssParser parser(db, id);
      parser.set_substitute_entities(true);
      try {
        parser.parse_file(id + ".auto.xml");
      } catch(xmlpp::parse_error e) {
	std::cerr << "Parsing error: " << e.what() << std::endl;
	return 6;
      }
    } else if (preparser.is_atom()) {
      cout << "(Atom feed)";
      AtomParser parser(db,id);
      parser.set_substitute_entities(true);
      try {
        parser.parse_file(id + ".auto.xml");
      } catch(xmlpp::parse_error e) {
        std::cerr << "Parsing error: " << e.what() << std::endl;
	return 6;
      }
    }

    /*
     * Finally, update etag and lastmodified.
     * It could be done earlier, but we would have no assurance that everything went right, and that we indeed updated. Better to do it that way.
     */
    db->update_timestamps_feed(id, *(headersRecipient->etag), *(headersRecipient->lastmodified));

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

  AbstractDB * db = init_database();

  std::list<std::vector<DBValue *> > entries = db->get_entries(id, 10);

  std::list<std::vector<DBValue *> >::iterator it;
  for (it = entries.begin(); it != entries.end(); ++it) {
    Entry e(it->at(0)->getStr(),it->at(1)->getStr(),it->at(2)->getStr(),it->at(3)->getInt(), it->at(4)->getStr());

    e.print();
  }
  
  return 0;
}

int list_feeds() {
  
  AbstractDB * db = init_database();

  std::list<struct feed *> * feeds = db->get_feeds();

  std::list<struct feed *>::iterator it;
  for (it = feeds->begin(); it != feeds->end(); ++it) {
    cout << " * " << *((*it)->id);
    cout << ": " << *((*it)->feed_url);
    cout << " (" << *((*it)->title);
    cout << ")" << endl;
  }
  
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
AbstractDB * 
init_database() {
  
  if (use_postgres) {
    PostgresDB * db = new PostgresDB();
    db->init_tables();
    return db;
  } else {
    return new Sqlite3DB();
  }
}
