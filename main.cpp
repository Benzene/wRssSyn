#include <cstdio>
#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include "rss2parser.h"

using namespace std;

static char errorBuffer[CURL_ERROR_SIZE];

int main (int argc, char* argv[]) {
  
  // Supposed to make the encoding right..
  std::locale::global(std::locale(""));
  
  ifstream fin("feedList.ini");
  
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
      
      
      if(!curl_easy_perform(curl)) {
	cout << errorBuffer << endl; 
      }
      
      curl_easy_cleanup(curl);
    }
    
    fclose(target);
    
    RssParser parser;
//    parser.set_substitute_entities(true); // ?
    parser.parse_file(id + ".auto.xml");

  }
  
  
  
  
  return 0;
  
}

