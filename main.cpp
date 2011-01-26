#include <libxml++/libxml++.h>
#include <libxml++/parsers/textreader.h>
#include <iostream>
#include "rss2parser.h"

using namespace std;

int main (int argc, char* argv[]) {
  
  // Supposed to make the encoding right..
  std::locale::global(std::locale(""));
  
  RssParser parser;
//  parser.set_substitute_entities(true); // ?
  parser.parse_file("rss_rorechut.xml");
  
  
  
  return 0;
  
}

