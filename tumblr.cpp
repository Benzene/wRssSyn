#include "tumblr.h"
#include "tumblr_auth.h"

using namespace std;

static char errorBuffer[CURL_ERROR_SIZE];

int
update_tumblr_feeds(sqlite3 * db) {
  
  /*
   * For each url in feedList.ini :
   *  - If file already exists : compute current file checksum.
   *  - Get the file, and replace the old one.
   *  - Compute new checksum. If unchanged, do nothing.
   *  - Read the file, update the feeds.
   */
  cout << "Updating tumblr dashboard feed" << endl;

  FILE * target = fopen ( "tumblrdashboard.auto.xml", "w");
    
  CURL *curl;
  CURLcode result;
    
  curl = curl_easy_init();
    
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
    curl_easy_setopt(curl, CURLOPT_URL, "http://www.tumblr.com/api/dashboard");
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    string postparameter = "email=" + tumblr_email + "&password=" + tumblr_password + "&num=20";
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postparameter.c_str());
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
    
/*
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
    cout << " done" << endl;
*/
  return 0;
  
}

