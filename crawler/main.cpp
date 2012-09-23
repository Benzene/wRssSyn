#include <cstdio>
#include <iostream>
#include <fstream>
#include <ctime>
#include <curl/curl.h>
#include "db.h"
#include "postgresdb.h"
#include "feed.h"
#include "rss2parser.h"
#include "atomparser.h"
#include "rssatomdecider.h"
//#include "tumblr.h"
//#include "tumblrparser.h"
#include "curl_helpers.h"
#include "time_helpers.h"

#include "config.h"

using namespace std;

static char errorBuffer[CURL_ERROR_SIZE];

AbstractDB * init_database();
int update_feeds();
int update_single_feed(int id);
int update_feed(AbstractDB * db, struct feed * f, bool force);
int get_feed(int id);
int list_feeds();
int add_feed(string url, string user);
int update_feed_url(int id, string newurl);
int add_user(string login);

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

  //std::cout << "Useragent will be: " << Config::get_instance()->val("useragent") << std::endl;
  
  if (argc < 2) {
    print_usage();
    return 1;
  } else {
    string cmd(argv[1]);
    if (cmd == "update") {
      if (argc < 3) {
        return update_feeds();
      } else {
        return update_single_feed(TimeHelpers::str2int(argv[2]));
      }
    } else if ( cmd == "get") {
      if (argc < 3) {
	print_usage();
	return 1;
      } else {
	return get_feed(TimeHelpers::str2int(argv[2]));
      }
    } else if ( cmd == "list") {
      return list_feeds();
    } else if ( cmd == "add") {
      if (argc < 4) {
	print_usage();
	return 1;
      } else {
	string url(argv[2]);
	string user(argv[3]);
	return add_feed(url,user);
      }
    } else if ( cmd == "update_url") {
      if (argc < 4) {
        print_usage();
	return 1;
      } else {
	    return update_feed_url(TimeHelpers::str2int(argv[2]),argv[3]);
      }
    } else if ( cmd == "useradd") {
      if (argc < 3) {
        print_usage();
        return 1;
      } else {
        string user(argv[2]);
        return add_user(user);
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
    cerr << "  update <feed_id> updates a single feed" << endl;
    cerr << "  get <feed_id>	prints last entries for feed <feed_id>" << endl;
    cerr << "  list		prints the list of feeds currently registered" << endl;
    cerr << "  add <url> <user> adds an rss url to the list" << endl; 
    cerr << "  update_url <feed_id> <new_url> updates the url of a feed" << endl;
    cerr << "  useradd <user> creates a new user" << endl;
}

int
parse_feed_file() {
  /*
   * This has become more or less useless. Could be useful as a standard OPML reader, though.
   */
  
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
    std::string user;
    fin >> url;
    fin >> id;
    fin >> user;
    if (fin.eof()) {
      break;
    }

    db->create_feed(FEED_RSS, url, user);
  }
  cout << "done." << endl;

  delete db;

  return 0;
}

int
update_feeds() {
  
  /* Initialise database connection */
  AbstractDB * db = init_database();

  /*
   * Update the already existing feeds.
   */
  std::list<struct feed *> * f = db->get_feeds();
  std::list<struct feed *>::iterator it;
  for (it = f->begin(); it != f->end(); ++it) {
    update_feed(db, (*it), false);
    delete (*it);
  }
  delete f;


  // TODO: free the memory of the feeds ?

  /*
   * Finally, parse the special purpose feeds.
   * Only tumblr is in there, for now.
   */
  /*
  std::string * target = update_tumblr_feeds(db);
  if (target != NULL) {
    TumblrParser parser(db);
    parser.set_substitute_entities(true);
    try {
      parser.parse_memory(*target);
    } catch(xmlpp::parse_error e) {
      std::cerr << "Parsing error on tumblr feed: " << e.what() << std::endl;
    }

    delete target;
  }
  */

  delete db;
  
  return 0;
  
}

int update_single_feed(int id) {

  AbstractDB * db = init_database();

  int retVal = -1;
  struct feed * f= db->get_feed_by_id(id);
  if (f != NULL) {
    retVal = update_feed(db, f, true);
    delete f;
  }

  delete db;

  return retVal;

}

int update_feed(AbstractDB * db, struct feed * f, bool force) {

  int uid = f->uid;
  std::string &url = f->feed_url;
  std::string &etag = f->etag;
  std::string &lastmodified = f->lastmodified;

  /* Empty urls are used for custom feeds. We can silently skip them. */	
  if (url.compare("") == 0) {
	  return 4;
  }

  string * target = new string("");
    
  CURL *curl;
  CURLcode result;
    
  curl = curl_easy_init();
    
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HEADER, 0);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, store_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, target);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, Config::get_instance()->val("useragent").c_str());
      
    /* Set the custom headers */
    struct curl_slist * slist = NULL;
    if (!force) {
      string ifmodifheader = "If-Modified-Since: ";
      ifmodifheader.append(lastmodified);
      string ifetagheader = "If-None-Match: ";
      ifetagheader.append(etag);
      slist = curl_slist_append(slist, ifmodifheader.c_str());
      slist = curl_slist_append(slist, ifetagheader.c_str());
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
    }

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
      
    if (!force) {
      curl_slist_free_all(slist);
    }
    curl_easy_cleanup(curl);
    
    cout << "Updating " << uid << " (" << url << ")" << endl;

    RssAtomDecider preparser;
    // Convert html entities to normal characters
    preparser.set_substitute_entities(true);
    try {
      preparser.parse_memory(*target);
    } catch(xmlpp::parse_error e) {
      std::cerr << "Preparsing error while parsing " << uid << " (" << url << "): " << e.what() << std::endl;
      std::cerr << "Document contains:" << std::endl;
      std::cerr << *target << std::endl;
      return 6;
    }
    if (preparser.is_rss2()) {
      cout << "(Rss feed)";
      RssParser parser(db, uid);
      parser.set_substitute_entities(true);
      try {
	parser.parse_memory(*target);
      } catch(xmlpp::parse_error e) {
	std::cerr << "Parsing error while parsing " << uid << " (" << url << "): " << e.what() << std::endl;
	return 6;
      }
    } else if (preparser.is_atom()) {
      cout << "(Atom feed)";
      AtomParser parser(db, uid);
      parser.set_substitute_entities(true);
      try {
	parser.parse_memory(*target);
      } catch(xmlpp::parse_error e) {
        std::cerr << "Parsing error while parsing " << uid << " (" << url << "): " << e.what() << std::endl;
	return 6;
      }
    }
    
    delete target;

    /*
     * Finally, update etag and lastmodified.
     * It could be done earlier, but we would have no assurance that everything went right, and that we indeed updated. Better to do it that way.
     */
    db->update_timestamps_feed(uid, *(headersRecipient->etag), *(headersRecipient->lastmodified));

    delete headersRecipient->etag;
    delete headersRecipient->lastmodified;
    delete headersRecipient;

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
get_feed(int id) {

  AbstractDB * db = init_database();

  std::list<Entry> entries = db->get_entries(id, 10);

  std::list<Entry>::iterator it;
  for (it = entries.begin(); it != entries.end(); ++it) {
    (*it).print();
  }

  delete db;
  
  return 0;
}

int list_feeds() {
  
  AbstractDB * db = init_database();

  std::list<struct feed *> * feeds = db->get_feeds();

  std::list<struct feed *>::iterator it;
  for (it = feeds->begin(); it != feeds->end(); ++it) {
    cout << " * " << (*it)->uid;
    cout << ": " << (*it)->feed_url;
    cout << " (" << (*it)->title;
    cout << ")" << endl;
    delete (*it);
  }

  delete feeds;
  delete db;
  
  return 0;
}

int add_feed(string url, string user) {

  AbstractDB * db = init_database();

  int id = db->create_feed(FEED_RSS, url, user);

  delete db;

  if (id != -1) {
    update_single_feed(id);
  }

  return 0;
}

int update_feed_url(int id, string newurl) {
  AbstractDB * db = init_database();

  db->update_feed_url(id, newurl);

  return 0;
}

int add_user(string login) {
  AbstractDB * db = init_database();

  db->add_user(login);

  return 0;
}


/*
 * Open the connection and create the tables if they don't already exist.
 */
AbstractDB * 
init_database() {
  
  PostgresDB * db = new PostgresDB();
  db->init_tables();
  return db;
}
