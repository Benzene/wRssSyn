#include "postgresdb.h"

PostgresDB::PostgresDB() {

}

PostgresDB::~PostgresDB() {

}

void
PostgresDB::create_feed(std::string &website_id, std::string &feed_url, std::string &user) {

}

void
PostgresDB::create_feed_full(std::string &website_id, std::string &feed_url, std::string &title, std::string &url, std::string &descr, std::string &imgtitle, std::string &imgurl, std::string &imglink, std::string &user, std::string &etag, std::string &lastmodified) {

}

std::list<struct feed *> * 
PostgresDB::get_feeds() {

	return new std::list<struct feed *>;
}

void
PostgresDB::update_timestamps_feed(std::string &website_id, std::string &etag, std::string &lastmodified) {

}

void
PostgresDB::update_metadata_feed(std::string &website_id, std::string &title, std::string &url, std::string &descr, std::string &imgtitle, std::string &imgurl, std::string &imglink) {

}

void
PostgresDB::insert_entry(std::string &website_id, std::string &id, std::string &title, std::string &link, int date, std::string &descr, std::string &user) {

}

std::list<std::vector<DBValue *> >
PostgresDB::get_entries(std::string &website_id, int num) {
	std::list<std::vector<DBValue *> > l;

	return l;
}
