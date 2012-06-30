#ifndef _WRSSSYN_POSTGRESDB_H_
#define _WRSSSYN_POSTGRESDB_H_

#include "db.h"
#include <string>
#include <list>
#include <vector>
#include <pqxx/pqxx>
#include "feed.h"
#include "entry.h"

class PostgresDB : public AbstractDB
{
  public:
    PostgresDB();
    virtual ~PostgresDB();

    std::string create_feed_full(std::string &website_id, std::string &feed_url, std::string &title, std::string &url, std::string &descr, std::string &imgtitle, std::string &imgurl, std::string &imglink, std::string &user, std::string &etag, std::string &lastmodified);
    std::list<struct feed *> * get_feeds();
    struct feed * get_feed_by_id(std::string &id);

    void update_timestamps_feed(std::string &website_id, std::string &etag, std::string &lastmodified);
    void update_metadata_feed(std::string &website_id, std::string &title, std::string &url, std::string &descr, std::string &imgtitle, std::string &imgurl, std::string &imglink);

    void update_feed_url(std::string &website_id, std::string &title);

    void insert_entry(std::string &website_id, std::string &id, std::string &title, std::string &link, int date, std::string &descr, std::string &user);
    std::list<Entry> get_entries(std::string &website_id, int num);

    void init_tables();

  private:

    pqxx::connection * db;


};

#endif
