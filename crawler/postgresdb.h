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

    int create_feed_full(const feed& feed, const std::string& user);
    std::list<struct feed *> * get_feeds();
    struct feed * get_feed_by_id(int id);

    void update_timestamps_feed(int id, std::string &etag, std::string &lastmodified);
    void update_metadata_feed(int id, std::string &title, std::string &url, std::string &descr, std::string &imgtitle, std::string &imgurl, std::string &imglink);

    void update_feed_url(int id, std::string &title);

    void insert_entry(int uid, std::string &id, std::string &title, std::string &link, int date, std::string &descr);
    std::list<Entry> get_entries(int id, int num);

    void add_user(std::string &user);

    void init_tables();

  private:

    struct feed * feed_from_pqxxresult(pqxx::result r, int k);

    pqxx::connection * db;


};

#endif
