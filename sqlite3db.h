#ifndef _WRSSSYN_DBSQLITE3_H_
#define _WRSSSYN_DBSQLITE3_H_

#include "db.h"
#include <string>
#include <sqlite3.h>
#include <list>
#include <vector>
#include "feed.h"

class Sqlite3Value : public DBValue {

  public:
    Sqlite3Value();
    Sqlite3Value(int v);
    Sqlite3Value(std::string str);
    Sqlite3Value(const char* str);
};

class Sqlite3DB : public AbstractDB
{
  public:
    Sqlite3DB();
    virtual ~Sqlite3DB();

    void create_feed_full(std::string &website_id, std::string &feed_url, std::string &title, std::string &url, std::string &descr, std::string &imgtitle, std::string &imgurl, std::string &imglink, std::string &user, std::string &etag, std::string &lastmodified);
    std::list<struct feed *> * get_feeds();
    void update_timestamps_feed(std::string &website_id, std::string &etag, std::string &lastmodified);
    void update_metadata_feed(std::string &website_id, std::string &title, std::string &url, std::string &descr, std::string &imgtitle, std::string &imgurl, std::string &imglink);

    void insert_entry(std::string &website_id, std::string &id, std::string &title, std::string &link, int date, std::string &descr, std::string &user);

    std::list<std::vector<DBValue *> > get_entries(std::string &website_id, int num);

  private:
    // Management functions. Might be something that can be made as a standalone wrapper ?
    sqlite3_stmt * buildStatement(std::string &query, std::vector<Sqlite3Value> &values, std::string debugPosition);
    void execNoReturnStmt(std::string &query, std::vector<Sqlite3Value> &values, std::string debugPosition);
    std::list<std::vector<Sqlite3Value *> > execReturnStmt(std::string &query, std::vector<Sqlite3Value> &values, std::vector<Sqlite3Value> &types, std::string debugPosition);

    sqlite3 * db;


};

#endif
