#include "postgresdb.h"
#include <iostream>
#include <cstdlib>
#include "time_helpers.h"
#include "config.h"

PostgresDB::PostgresDB() {
  /* Open the connection the database. */
    try {
        db = new pqxx::connection("dbname=" + Config::get_instance()->val("db_name") + " user=" + Config::get_instance()->val("db_user"));
    } catch (pqxx::pqxx_exception const& exc) {
        std::cerr << "Connection to database failed ! Check your parameters !" << std::endl;
        exit(-1);
    }

}

PostgresDB::~PostgresDB() {
  delete db;

}

void
PostgresDB::init_tables() {
  pqxx::work txn(*db);

  try {
    std::string qSources("CREATE TABLE IF NOT EXISTS sources "
		  "(website_id TEXT PRIMARY KEY, "
		  "feed_url TEXT NOT NULL UNIQUE, "
		  "title TEXT NOT NULL, "
		  "url TEXT NOT NULL, "
		  "descr TEXT NOT NULL, "
		  "etag TEXT, "
		  "lastmodified TEXT)");
    txn.exec(qSources);

    std::string qPosts("CREATE TABLE IF NOT EXISTS posts "
		  "(uid SERIAL PRIMARY KEY, "
		  "website_id TEXT REFERENCES sources (website_id), "
		  "id TEXT NOT NULL, "
		  "title TEXT NOT NULL, "
		  "link TEXT NOT NULL, "
		  "date TIMESTAMP NOT NULL, "
		  "description TEXT NOT NULL, "
		  "UNIQUE (website_id, id))");
    txn.exec(qPosts);

    std::string qUsers("CREATE TABLE IF NOT EXISTS users "
          "(uid SERIAL PRIMARY KEY, "
		  "login TEXT NOT NULL UNIQUE)");
    /*
		  "pass TEXT NOT NULL, "
		  "tumblr_email TEXT, "
		  "tumblr_pass TEXT)");
          */
    txn.exec(qUsers);

    std::string qReads("CREATE TABLE IF NOT EXISTS readeds "
		  "(\"user\" TEXT REFERENCES users (login), "
		  "post_id INTEGER REFERENCES posts (uid), "
		  "PRIMARY KEY (\"user\", post_id))");
    txn.exec(qReads);

    std::string qSubscriptions("CREATE TABLE IF NOT EXISTS subscriptions "
		  "(\"user\" TEXT REFERENCES users (login), "
		  "sources_id TEXT REFERENCES sources (website_id), "
		  "PRIMARY KEY (\"user\", sources_id))");
    txn.exec(qSubscriptions);

    std::string qFavorites("CREATE TABLE IF NOT EXISTS favorites "
		  "(\"user\" TEXT REFERENCES users (login), "
		  "post_id INTEGER REFERENCES posts (uid), "
  		  "PRIMARY KEY (\"user\", post_id))");
    txn.exec(qFavorites);

    std::string qDisabled("CREATE TABLE IF NOT EXISTS disabled "
            "(sources_id TEXT REFERENCES sources (website_id), "
            "PRIMARY KEY (sources_id))");
    txn.exec(qDisabled);

    txn.commit();

  } catch (pqxx::pqxx_exception const& exc) {
    std::cerr << "Exception !" << std::endl;
    std::cerr << exc.base().what() << std::endl;
    exit(-1);
  }

}

std::string
PostgresDB::create_feed_full(std::string &website_id, std::string &feed_url, std::string &title, std::string &url, std::string &descr, std::string &imgtitle, std::string &imgurl, std::string &imglink, std::string &user, std::string &etag, std::string &lastmodified) {

  /* We have to do two things:
   * - create the feed, if we don't already have it.
   * - subscribe the user to it.
   */

  pqxx::work txn(*db);
  
  std::string uid = "";

  try {

    std::string exist_query("SELECT website_id "
		  "FROM sources "
		  "WHERE feed_url=" + txn.quote(feed_url));

    pqxx::result exist_r = txn.exec(exist_query);

    if(exist_r.size() == 0) {

      std::string insert_query("INSERT INTO sources "
		  "(website_id, feed_url, title, url, descr, etag, lastmodified) VALUES ("
		  + txn.quote(website_id) + ","
		  + txn.quote(feed_url) + ","
		  + txn.quote(title) + ","
		  + txn.quote(url) + ","
		  + txn.quote(descr) + ","
		  + txn.quote(etag) + ","
		  + txn.quote(lastmodified) + ")"
	  );
      txn.exec(insert_query);

      uid = website_id;
    } else {
      uid = exist_r[0][0].as<std::string>();
    }

    std::string subscr_query("INSERT INTO subscriptions "
		    "(\"user\", sources_id) VALUES "
		    "(" + txn.quote(user) + "," + txn.quote(uid)
		    + ")");
    txn.exec(subscr_query);

    txn.commit();

  } catch (pqxx::unique_violation const& exc) {
    /* This only means that the sources is already there. */
//    std::cerr << "Note: source already present in database." << std::endl;
  } catch (pqxx::pqxx_exception const& exc) {
    std::cerr << "Exception while creating feed !" << std::endl;
    std::cerr << exc.base().what() << std::endl;
    exit(-1);
  }

  return uid;

}

std::list<struct feed *> * 
PostgresDB::get_feeds() {
  pqxx::work txn(*db);

  std::list<struct feed *> * externalL = new std::list<struct feed *>;

  try {
    pqxx::result r = txn.exec(
		  "SELECT website_id, feed_url, title, etag, lastmodified FROM sources WHERE website_id NOT IN (SELECT sources_id FROM disabled)");
    txn.commit();

    std::cout << r.size() << " feeds found in database." << std::endl;

    for(pqxx::result::size_type i = 0; i < r.size(); ++i) {
      struct feed * f = new struct feed;
      f->id = r[i]["website_id"].as<std::string>();
      f->feed_url = r[i]["feed_url"].as<std::string>();
      f->title = r[i]["title"].as<std::string>();
      f->etag = r[i]["etag"].as<std::string>();
      f->lastmodified = r[i]["lastmodified"].as<std::string>();
      externalL->push_back(f);
    }

  } catch (pqxx::pqxx_exception const& exc) {
    std::cerr << "Exception while getting list of feeds !" << std::endl;
    std::cerr << exc.base().what() << std::endl;
    exit(-1);
  }
  return externalL;

}

struct feed *
PostgresDB::get_feed_by_id(std::string &id) {
  pqxx::work txn(*db);

  try {
    pqxx::result r = txn.exec(
          "SELECT website_id, feed_url, title, etag, lastmodified FROM sources WHERE website_id = "   
          + txn.quote(id));
    txn.commit();

    if (r.size() == 0) {
        // Not found
        std::cout << "Feed " << id << " not found.";
        return NULL;
    } else {
        if (r.size() > 1) {
        // Multiple matches
        std::cout << "More than one match for id " << id << ". This shouldn't happen.";
        }
        struct feed * f = new struct feed;
        f->id = r[0]["website_id"].as<std::string>();
        f->feed_url = r[0]["feed_url"].as<std::string>();
        f->title = r[0]["title"].as<std::string>();
        f->etag = r[0]["etag"].as<std::string>();
        f->lastmodified = r[0]["lastmodified"].as<std::string>();

        return f;
    }
  } catch (pqxx::pqxx_exception const& exc) {
    std::cerr << "Exception while getting a feed !" << std::endl;
    std::cerr << exc.base().what() << std::endl;
    exit(-1);
  }

}


void
PostgresDB::update_timestamps_feed(std::string &website_id, std::string &etag, std::string &lastmodified) {
  pqxx::work txn(*db);

  std::string query("UPDATE sources SET "
		  "etag=" + txn.quote(etag) + ", "
		  "lastmodified=" + txn.quote(lastmodified) + " "
		  "WHERE website_id=" + txn.quote(website_id));

  try {
    txn.exec(query);
    txn.commit();
  } catch (pqxx::pqxx_exception const& exc) {
    std::cerr << "Exception while updating Etag/Lastmodified !" << std::endl;
    std::cerr << exc.base().what() << std::endl;
    exit(-1);
  }

}

void
PostgresDB::update_metadata_feed(std::string &website_id, std::string &title, std::string &url, std::string &descr, std::string &imgtitle, std::string &imgurl, std::string &imglink) {
  pqxx::work txn(*db);
  std::string query("UPDATE sources SET "
		  "title=" + txn.quote(title) + ", "
		  "url=" + txn.quote(url) + ", "
		  "descr=" + txn.quote(descr) + " "
		  "WHERE website_id=" + txn.quote(website_id));

  try {
    txn.exec(query);
    txn.commit();
  } catch (pqxx::pqxx_exception const& exc) {
    std::cerr << "Exception while updating feed's metadata !" << std::endl;
    std::cerr << exc.base().what() << std::endl;
    exit(-1);
  }

}

void
PostgresDB::update_feed_url(std::string &website_id, std::string &feed_url) {
  pqxx::work txn(*db);
  std::string query("UPDATE sources SET "
		  "feed_url=" + txn.quote(feed_url) + " "
		  "WHERE website_id=" + txn.quote(website_id));

  try {
    txn.exec(query);
    txn.commit();
  } catch (pqxx::pqxx_exception const& exc) {
    std::cerr << "Exception while updating feed's metadata !" << std::endl;
    std::cerr << exc.base().what() << std::endl;
    exit(-1);
  }
}

void
PostgresDB::insert_entry(std::string &website_id, std::string &id, std::string &title, std::string &link, int date, std::string &descr) {
  pqxx::work txn(*db);
  std::string d = TimeHelpers::getPGREInput(date);
  std::string query("INSERT INTO posts "
		  "(website_id, id, title, link, date, description) "
		  "VALUES ("
		  + txn.quote(website_id) + ", "
		  + txn.quote(id) + ", "
		  + txn.quote(title) + ", "
		  + txn.quote(link) + ", "
		  + txn.quote(d) + ", "
		  + txn.quote(descr) + ")");

  try {
    txn.exec(query);
    txn.commit();
  } catch (pqxx::unique_violation const& exc) {
//    std::cerr << "Note: post already exists. Skipping." << std::endl;
  } catch (pqxx::pqxx_exception const& exc) {
    std::cerr << "Exception while inserting entry !" << std::endl;
    std::cerr << exc.base().what() << std::endl;
    exit(-1);
  }

}

std::list<Entry>
PostgresDB::get_entries(std::string &website_id, int num) {
  pqxx::work txn(*db);
  std::list<Entry> l;

  std::string query("SELECT id, title, link, date, description "
		  "FROM posts "
		  "WHERE website_id=" + txn.quote(website_id) + " "
		  "ORDER BY date DESC LIMIT " + txn.quote(num));

  try {
    pqxx::result r = txn.exec(query);
    txn.commit();

    std::cout << r.size() << " entries found for feed " << website_id << std::endl;

    for (pqxx::result::size_type i = 0; i < r.size(); ++i) {
      std::string date = r[i][3].as<std::string>();
      int date_i = TimeHelpers::stampFromPGRE(date);

      Entry e(r[i][0].as<std::string>(),
		      r[i][1].as<std::string>(),
		      r[i][2].as<std::string>(),
		      date_i,
		      r[i][4].as<std::string>());
      l.push_back(e);
    }
  } catch (pqxx::pqxx_exception const& exc) {
    std::cerr << "Exception while getting entries for " << website_id << std::endl;
    std::cerr << exc.base().what() << std::endl;
    exit(-1);
  }

  return l;

}

void
PostgresDB::add_user(std::string &user) {
    pqxx::work txn(*db);

    std::string query("INSERT INTO users (login) "
        "VALUES (" + txn.quote(user) + ")");

  try {
    txn.exec(query);
    txn.commit();
  } catch (pqxx::pqxx_exception const& exc) {
    std::cerr << "Exception while inserting entry !" << std::endl;
    std::cerr << exc.base().what() << std::endl;
    exit(-1);
  }

}


