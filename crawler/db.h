#ifndef _WRSSSYN_DB_H_
#define _WRSSSYN_DB_H_

#include <string>
#include <list>
#include <vector>
#include "feed.h"
#include "entry.h"
#include "tumblr_entry.h"

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

    /* Creates a feed with lots of default values */
    virtual int create_feed(const feed_type& feed_type, const std::string& feed_url, const std::string& user);
    /* Create a feed with all details specified */
    virtual int create_feed_full(const feed& feed, const std::string &user) = 0;

    /* Get the list of feeds */
    // The caller is responsible for freeing the feeds.
    virtual std::list<struct feed *> * get_feeds() =0;

    /* Get one feed by id */
    // Returns null if the feed isn't found.
    virtual struct feed * get_feed_by_id(int id) =0;


    /* Update the timestamp/etag of a feed, for saving bandwidth */
    virtual void update_timestamps_feed(int id, std::string &etag, std::string &lastmodified) =0;

    /* Update the metadata of a feed (title, url, description, ...) */
    virtual void update_metadata_feed(int id, std::string &title, std::string &url, std::string &descr, std::string &imgtitle, std::string &imgurl, std::string &imglink) =0;

    /* Change the url of a feed */
    virtual void update_feed_url(int id, std::string &feed_url) =0;


    /* Insert an entry in the db */
    void insert_entry(int id, Entry &entry);
    //void insert_entry(std::string &website_id, TumblrEntry &entry);

    /* List existing entries */
    virtual std::list<Entry> get_entries(int id, int num) =0;

    /* Add user */
    virtual void add_user(std::string &user) =0;


  protected:
    /* Insert an entry (this function is the ones that has to be
     * overriden) */
    virtual void insert_entry(int uid, std::string &id, std::string &title, std::string &link, int date, std::string &descr) =0;
};

#endif
