#include <string>
#include "tumblr.h"
#include "auth.h"
#include "curl_helpers.h"

using namespace std;

static char errorBuffer[CURL_ERROR_SIZE];

std::string *
update_tumblr_feeds(AbstractDB * db) {
  
  /*
   * For each url in feedList.ini :
   *  - If file already exists : compute current file checksum.
   *  - Get the file, and replace the old one.
   *  - Compute new checksum. If unchanged, do nothing.
   *  - Read the file, update the feeds.
   */
  cout << "Updating tumblr dashboard feed" << endl;

  /* Make sure the infos are in the database. */
  string id("tumblrdashboard");
  string title("Tumblr dashboard");
  string url("http://www.tumblr.com/dashboard");
  string blank("");
  db->create_feed_full(id, blank, title, url, title, blank, blank, blank, glob_login,blank,blank);

  //FILE * target = fopen ( "tumblrdashboard.auto.xml", "w");
  std::string * target = new std::string("");
    
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
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, store_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, target);
      
    result = curl_easy_perform(curl);
    if(result != 0) {
      cout << "Connection error : " << errorBuffer << endl; 
      return NULL;
    }
      
    curl_easy_cleanup(curl);
  }
    
  return target;
  
}

