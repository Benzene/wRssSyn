#ifndef _WRSSSYN_DB_H_
#define _WRSSSYN_DB_H_

#include <string>
#include <list>
#include <vector>
#include "feed.h"

class DBValue {
  public:
    DBValue();
    DBValue(int v);
    DBValue(std::string str);
    DBValue(const char * str);

    int getType();
    std::string getStr();
    int getInt();

    static const int DBVAL_EMPTY = 0;
    static const int DBVAL_INT = 1;
    static const int DBVAL_STR = 2;

  private:
    int type;
    int v;
    std::string str;
};

class AbstractDB
{
  public:
    AbstractDB();
    virtual ~AbstractDB();

    virtual void create_feed(std::string &website_id, std::string &feed_url, std::string &user);
    virtual void create_feed_full(std::string &website_id, std::string &feed_url, std::string &title, std::string &url, std::string &descr, std::string &imgtitle, std::string &imgurl, std::string &imglink, std::string &user, std::string &etag, std::string &lastmodified) =0;
    virtual std::list<struct feed *> * get_feeds() =0;

    virtual void update_timestamps_feed(std::string &website_id, std::string &etag, std::string &lastmodified) =0;
    virtual void update_metadata_feed(std::string &website_id, std::string &title, std::string &url, std::string &descr, std::string &imgtitle, std::string &imgurl, std::string &imglink) =0;

    virtual void insert_entry(std::string &website_id, std::string &id, std::string &title, std::string &link, int date, std::string &descr, std::string &user) =0;
    virtual std::list<std::vector<DBValue *> > get_entries(std::string &website_id, int num) =0;

};

#endif
