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

  /* TODO: interrupting this might have strange consequences (though the transaction should rollback by itself). Maybe the real initialization belongs
   * somewhere else ? */

  try {
    std::string qCheckType("SELECT 1 FROM pg_type WHERE typname='feed_type'");
    pqxx::result exist_r = txn.exec(qCheckType);
    if (exist_r.size() == 0) {
        std::string qTypeCreate("CREATE TYPE feed_type AS ENUM ('rss', 'tumblr')");
        txn.exec(qTypeCreate);
    }

    std::string qSources("CREATE TABLE IF NOT EXISTS sources "
          "(uid SERIAL PRIMARY KEY, "  
          "type FEED_TYPE NOT NULL, "
		  /*"website_id TEXT NOT NULL, "*/
		  "feed_url TEXT NOT NULL, "
		  "title TEXT NOT NULL, "
		  "url TEXT NOT NULL, "
		  "descr TEXT NOT NULL, "
		  "etag TEXT, "
		  "lastmodified TEXT, "
          "UNIQUE (type, feed_url))");
    txn.exec(qSources);

    std::string qPosts("CREATE TABLE IF NOT EXISTS posts "
		  "(uid SERIAL PRIMARY KEY, "
		  "source_id INTEGER REFERENCES sources (uid), "
		  "id TEXT NOT NULL, "
		  "title TEXT NOT NULL, "
		  "link TEXT NOT NULL, "
		  "date TIMESTAMP NOT NULL, "
		  "description TEXT NOT NULL, "
		  "UNIQUE (source_id, id))");
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
		  "(\"user\" INTEGER REFERENCES users (uid), "
		  "post_id INTEGER REFERENCES posts (uid), "
		  "PRIMARY KEY (\"user\", post_id))");
    txn.exec(qReads);

    std::string qSubscriptions("CREATE TABLE IF NOT EXISTS subscriptions "
		  "(\"user\" INTEGER REFERENCES users (uid), "
		  "sources_id INTEGER REFERENCES sources (uid), "
		  "PRIMARY KEY (\"user\", sources_id))");
    txn.exec(qSubscriptions);

    std::string qFavorites("CREATE TABLE IF NOT EXISTS favorites "
		  "(\"user\" INTEGER REFERENCES users (uid), "
		  "post_id INTEGER REFERENCES posts (uid), "
  		  "PRIMARY KEY (\"user\", post_id))");
    txn.exec(qFavorites);

    std::string qDisabled("CREATE TABLE IF NOT EXISTS disabled "
            "(sources_id INTEGER REFERENCES sources (uid), "
            "PRIMARY KEY (sources_id))");
    txn.exec(qDisabled);

    txn.commit();

  } catch (pqxx::pqxx_exception const& exc) {
    std::cerr << "Exception !" << std::endl;
    std::cerr << exc.base().what() << std::endl;
    exit(-1);
  }

}

int
PostgresDB::create_feed_full(const feed& feed, const std::string& user) {

  /* We have to do two things:
   * - create the feed, if we don't already have it.
   * - subscribe the user to it.
   */

  pqxx::work txn(*db);
  
  int uid = -1;

  try {

    std::string exist_query("SELECT uid "
		  "FROM sources "
		  "WHERE feed_url=" + txn.quote(feed.feed_url));

    pqxx::result exist_r = txn.exec(exist_query);

    if(exist_r.size() == 0) {

      std::string type = "";
      if (feed.type == FEED_RSS) {
          type = "rss";
      } else if (feed.type == FEED_TUMBLR) {
          type = "tumblr";
      }

      std::string insert_query("INSERT INTO sources "
		  "(type, feed_url, title, url, descr, etag, lastmodified) VALUES ("
		  + txn.quote(type) + ","
		  + txn.quote(feed.feed_url) + ","
		  + txn.quote(feed.title) + ","
		  + txn.quote(feed.url) + ","
		  + txn.quote(feed.descr) + ","
		  + txn.quote(feed.etag) + ","
		  + txn.quote(feed.lastmodified) + ")"
	  );
      txn.exec(insert_query);

      exist_r = txn.exec(exist_query);

      if (exist_r.size() == 0) {
          std::cerr << "Feed insertion silently failed. That's weird. Exiting." << std::endl;
          exit(-1);
      }
    }

    uid = exist_r[0][0].as<int>();

    std::string subscr_query("INSERT INTO subscriptions "
		    "(\"user\", sources_id) "
		    "(SELECT uid, " + txn.quote(uid) + " FROM users WHERE login=" + txn.quote(user) + " LIMIT 1)");
    txn.exec(subscr_query);

    /* TODO: the "User doesn't exist" case will silently fail. Fix that. */

    txn.commit();

  } catch (pqxx::unique_violation const& exc) {
    /* This only means that the source is already there. */
    uid = -1;
//    std::cerr << "Note: source already present in database." << std::endl;
  } catch (pqxx::pqxx_exception const& exc) {
    std::cerr << "Exception while creating feed !" << std::endl;
    std::cerr << exc.base().what() << std::endl;
    exit(-1);
  }

  return uid;

}

struct feed *
PostgresDB::feed_from_pqxxresult(pqxx::result r, int k) {
      struct feed * f = new struct feed;
      f->uid = r[k]["uid"].as<int>();

      std::string feed_type(r[k]["type"].as<std::string>());
      if (feed_type == "rss") {
          f->type = FEED_RSS;
      } else if (feed_type == "tumblr") {
          f->type = FEED_TUMBLR;
      } else {
          f->type = FEED_UNK;
      }

      f->feed_url = r[k]["feed_url"].as<std::string>();
      f->title = r[k]["title"].as<std::string>();
      f->url = r[k]["url"].as<std::string>();
      f->descr = r[k]["descr"].as<std::string>();
      f->etag = r[k]["etag"].as<std::string>();
      f->lastmodified = r[k]["lastmodified"].as<std::string>();

      return f;
}

std::list<struct feed *> * 
PostgresDB::get_feeds() {
  pqxx::work txn(*db);

  std::list<struct feed *> * externalL = new std::list<struct feed *>;

  try {
    pqxx::result r = txn.exec(
		  "SELECT uid, type, feed_url, title, url, descr, etag, lastmodified FROM sources WHERE uid NOT IN (SELECT sources_id FROM disabled)");
    txn.commit();

    std::cout << r.size() << " feeds found in database." << std::endl;

    for(pqxx::result::size_type i = 0; i < r.size(); ++i) {
      
      externalL->push_back(feed_from_pqxxresult(r,i));
    }

  } catch (pqxx::pqxx_exception const& exc) {
    std::cerr << "Exception while getting list of feeds !" << std::endl;
    std::cerr << exc.base().what() << std::endl;
    exit(-1);
  }
  return externalL;

}

struct feed *
PostgresDB::get_feed_by_id(int id) {
  pqxx::work txn(*db);

  try {
    pqxx::result r = txn.exec(
          "SELECT uid, type, feed_url, title, url, descr, etag, lastmodified FROM sources WHERE uid= "   
          + txn.quote(id));
    txn.commit();

    if (r.size() == 0) {
        // Not found
        std::cout << "Feed " << id << " not found.";
        return NULL;
    } else {
        if (r.size() > 1) {
        // Multiple matches
        std::cout << "Warning: more than one match for id " << id << ". This shouldn't happen.";
        }
        return feed_from_pqxxresult(r,0);
    }
  } catch (pqxx::pqxx_exception const& exc) {
    std::cerr << "Exception while getting a feed !" << std::endl;
    std::cerr << exc.base().what() << std::endl;
    exit(-1);
  }

}


void
PostgresDB::update_timestamps_feed(int uid, std::string &etag, std::string &lastmodified) {
  pqxx::work txn(*db);

  std::string query("UPDATE sources SET "
		  "etag=" + txn.quote(etag) + ", "
		  "lastmodified=" + txn.quote(lastmodified) + " "
		  "WHERE uid=" + txn.quote(uid));

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
PostgresDB::update_metadata_feed(int uid, std::string &title, std::string &url, std::string &descr, std::string&, std::string&, std::string&) {
  pqxx::work txn(*db);
  std::string query("UPDATE sources SET "
		  "title=" + txn.quote(title) + ", "
		  "url=" + txn.quote(url) + ", "
		  "descr=" + txn.quote(descr) + " "
		  "WHERE uid=" + txn.quote(uid));

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
PostgresDB::update_feed_url(int uid, std::string &feed_url) {
  pqxx::work txn(*db);
  std::string query("UPDATE sources SET "
		  "feed_url=" + txn.quote(feed_url) + " "
		  "WHERE uid=" + txn.quote(uid));

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
PostgresDB::insert_entry(int uid, std::string &id, std::string &title, std::string &link, int date, std::string &descr) {
  pqxx::work txn(*db);
  std::string d = TimeHelpers::getPGREInput(date);
  std::string query("INSERT INTO posts "
		  "(source_id, id, title, link, date, description) "
		  "VALUES ("
		  + txn.quote(uid) + ", "
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
PostgresDB::get_entries(int uid, int num) {
  pqxx::work txn(*db);
  std::list<Entry> l;

  std::string query("SELECT id, title, link, date, description "
		  "FROM posts "
		  "WHERE source_id=" + txn.quote(uid) + " "
		  "ORDER BY date DESC LIMIT " + txn.quote(num));

  try {
    pqxx::result r = txn.exec(query);
    txn.commit();

    std::cout << r.size() << " entries found for feed " << uid << std::endl;

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
    std::cerr << "Exception while getting entries for " << uid << std::endl;
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


