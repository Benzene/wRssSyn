#include "sqlite3db.h"
#include <iostream>

Sqlite3Value::Sqlite3Value() : DBValue() {}
Sqlite3Value::Sqlite3Value(int v) : DBValue(v) {}
Sqlite3Value::Sqlite3Value(std::string str) : DBValue(str) {}
Sqlite3Value::Sqlite3Value(const char * str) : DBValue(str) {}

Sqlite3DB::Sqlite3DB() {

	// TODO: The open read write for all the time the program is ran might lock everything. To be investigated.
	
  /* Get a handle on the database */
  std::string dbName = "feeds.db";
  sqlite3_open_v2(dbName.c_str(), &db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE, NULL);


  /* Create the tables if they don't already exist. */
  std::string query("CREATE TABLE IF NOT EXISTS posts (website_id TEXT, id TEXT, title TEXT, link TEXT, date INTEGER, description TEXT, read INTEGER, user TEXT, PRIMARY KEY (website_id, id))");
  std::vector<Sqlite3Value> empty;
  execNoReturnStmt(query, empty, "[Init database]");

  std::string query2("CREATE TABLE IF NOT EXISTS sources (website_id TEXT PRIMARY KEY, feed_url TEXT, title TEXT, url TEXT, descr TEXT, imgtitle TEXT, imgurl TEXT, imglink TEXT, user TEXT, etag TEXT, lastmodified TEXT)");
  execNoReturnStmt(query2, empty, "[Init database]");

}

Sqlite3DB::~Sqlite3DB() {

}

sqlite3_stmt * Sqlite3DB::buildStatement(std::string &query, std::vector<Sqlite3Value> &values, std::string debugPosition) {
  sqlite3_stmt * sq_stmt;
  int retcode = 0;

  retcode = sqlite3_prepare_v2(db, query.c_str(), -1, &sq_stmt, NULL);
  if (retcode != SQLITE_OK) {
    std::cerr << "Prepare() failed at " << debugPosition << " (" << retcode << ")" << std::endl;
  }

  for (int i = 0; i < values.size(); ++i) {
    Sqlite3Value * val = &(values.at(i));
    if (val->getType() == DBValue::DBVAL_INT) {
      retcode = sqlite3_bind_int(sq_stmt, i+1, val->getInt());
    } else if (val->getType() == DBValue::DBVAL_STR) {
      retcode = sqlite3_bind_text(sq_stmt, i+1, values.at(i).getStr().c_str(), -1, SQLITE_STATIC);
    } else {
      std::cerr << "Malformed Sqlite3Value in parameters !" << std::endl;
      retcode = 0;
    }

    if (retcode != SQLITE_OK) {
      std::cerr << "Bind failed at " << debugPosition << " (" << retcode << ")" << std::endl;
    }
  }

  return sq_stmt;
}

void Sqlite3DB::execNoReturnStmt(std::string &query, std::vector<Sqlite3Value> &values, std::string debugPosition) {

  sqlite3_stmt * sq_stmt = buildStatement(query, values, debugPosition);

  sqlite3_step(sq_stmt);
  sqlite3_finalize(sq_stmt);
}

std::list<std::vector<Sqlite3Value *> > Sqlite3DB::execReturnStmt(std::string &query, std::vector<Sqlite3Value> &values, std::vector<Sqlite3Value> &types, std::string debugPosition) {
  sqlite3_stmt * sq_stmt = buildStatement(query, values, debugPosition);

  std::list<std::vector<Sqlite3Value *> > l;
  while ( sqlite3_step(sq_stmt) == SQLITE_ROW) {
    std::vector<Sqlite3Value *> v(types.size());
    for (int i = 0; i < v.size(); ++i) {
      if (types.at(i).getType() == DBValue::DBVAL_INT) {
	v[i] = new Sqlite3Value(sqlite3_column_int(sq_stmt,i));
      } else if (types.at(i).getType() == DBValue::DBVAL_STR) {
        v[i] = new Sqlite3Value((char*)sqlite3_column_text(sq_stmt,i));
      } else {
	std::cerr << "Malformed Sqlite3Value in return type !" << std::endl;
      }
    }
    l.push_back(v);
  }

  sqlite3_finalize(sq_stmt);

  return l;
}

void Sqlite3DB::create_feed(std::string &website_id, std::string &feed_url, std::string &user) {

  std::string blank("");
  
  create_feed_full(website_id, feed_url, blank, blank, blank, blank, blank, blank, user, blank, blank);
}

void Sqlite3DB::create_feed_full(std::string &website_id, std::string &feed_url, std::string &title, std::string &url, std::string &descr, std::string &imgtitle, std::string &imgurl, std::string &imglink, std::string &user, std::string &etag, std::string &lastmodified) {

  std::string query("INSERT OR IGNORE INTO sources (website_id, feed_url, title, url, descr, imgtitle, imgurl, imglink, user, etag, lastmodified) VALUES (?,?,?,?,?,?,?,?,?,?,?)");
  std::vector<Sqlite3Value> values(11);
  values[0] = Sqlite3Value(website_id);
  values[1] = Sqlite3Value(feed_url);
  values[2] = Sqlite3Value(title);
  values[3] = Sqlite3Value(url);
  values[4] = Sqlite3Value(descr);
  values[5] = Sqlite3Value(imgtitle);
  values[6] = Sqlite3Value(imgurl);
  values[7] = Sqlite3Value(imglink);
  values[8] = Sqlite3Value(user);
  values[9] = Sqlite3Value(etag);
  values[10] = Sqlite3Value(lastmodified);

  execNoReturnStmt(query, values, "create_feed_full()");

}

std::list<struct feed *> * Sqlite3DB::get_feeds() {

	// TODO: Make this use the execReturnStmt stuff.
  std::string query("SELECT website_id, feed_url, title, etag, lastmodified FROM sources");
  std::vector<Sqlite3Value> values;

  std::vector<Sqlite3Value> types(5);
  types[0] = Sqlite3Value("");
  types[1] = Sqlite3Value("");
  types[2] = Sqlite3Value("");
  types[3] = Sqlite3Value("");
  types[4] = Sqlite3Value("");

  execReturnStmt(query, values, types, "[get_feeds()]");

  std::list<std::vector<Sqlite3Value *> > internalL = execReturnStmt(query, values, types, "[Get entries]");

  std::list<struct feed *> * externalL = new std::list<struct feed *>;

  std::list<std::vector<Sqlite3Value *> >::iterator it;
  for (it = internalL.begin(); it != internalL.end(); ++it) {
    struct feed * f = new struct feed;
    f->id = new std::string(it->at(0)->getStr());
    f->feed_url = new std::string(it->at(1)->getStr());
    f->title = new std::string(it->at(2)->getStr());
    f->etag = new std::string(it->at(3)->getStr());
    f->lastmodified = new std::string(it->at(4)->getStr());
    externalL->push_back(f);
  }

  return externalL;
}

void Sqlite3DB::update_timestamps_feed(std::string &website_id, std::string &etag, std::string &lastmodified) {
  std::string query("UPDATE sources SET etag=?, lastmodified=? WHERE website_id=?");

  std::vector<Sqlite3Value> values(3);
  values[0] = Sqlite3Value(etag);
  values[1] = Sqlite3Value(lastmodified);
  values[2] = Sqlite3Value(website_id);

  execNoReturnStmt(query, values, "[Update Etag/LastModified]");
}

void Sqlite3DB::update_metadata_feed(std::string &website_id, std::string &title, std::string &url, std::string &descr, std::string &imgtitle, std::string &imgurl, std::string &imglink) {
  
  std::string query("UPDATE sources SET title=?, url=?, descr=?, imgtitle=?, imgurl=?, imglink=? WHERE website_id=?");

  std::vector<Sqlite3Value> values(7);
  values[0] = Sqlite3Value(title);
  values[1] = Sqlite3Value(url);
  values[2] = Sqlite3Value(descr);
  values[3] = Sqlite3Value(imgtitle);
  values[4] = Sqlite3Value(imgurl);
  values[5] = Sqlite3Value(imglink);
  values[6] = Sqlite3Value(website_id);

  execNoReturnStmt(query, values, "[Update source metadata]");

}

void Sqlite3DB::insert_entry(std::string &website_id, std::string &id, std::string &title, std::string &link, int date, std::string &descr, std::string &user) {

  std::string query("INSERT OR IGNORE INTO posts VALUES (?,?,?,?,?,?,0,?)");

  std::vector<Sqlite3Value> values(7);
  values[0] = Sqlite3Value(website_id);
  values[1] = Sqlite3Value(id);
  values[2] = Sqlite3Value(title);
  values[3] = Sqlite3Value(link);
  values[4] = Sqlite3Value(date);
  values[5] = Sqlite3Value(descr);
  values[6] = Sqlite3Value(user);

  execNoReturnStmt(query, values, "[Insert entry]");

}

std::list<std::vector<DBValue *> > Sqlite3DB::get_entries(std::string &website_id, int num) {
  //std::string query("SELECT id, title, link, date, descr FROM posts WHERE website_id=? ORDER BY date DESC LIMIT ?");
  std::string query("SELECT id, title, link, date, description FROM posts WHERE website_id=? ORDER BY date DESC LIMIT ?");

  std::vector<Sqlite3Value> values(2);
  values[0] = Sqlite3Value(website_id);
  values[1] = Sqlite3Value(num);

  std::vector<Sqlite3Value> types(5);
  types[0] = Sqlite3Value("");
  types[1] = Sqlite3Value("");
  types[2] = Sqlite3Value("");
  types[3] = Sqlite3Value(0);
  types[4] = Sqlite3Value("");

  /* There might be a way to make this cast more quickly. For now, we will have to manually make Sqlite3Value:s DBValue:s */
  std::list<std::vector<Sqlite3Value *> > internalL = execReturnStmt(query, values, types, "[Get entries]");

  std::list<std::vector<DBValue *> > externalL;

  std::list<std::vector<Sqlite3Value *> >::iterator it;
  for(it = internalL.begin(); it != internalL.end(); ++it) {
    std::vector<DBValue *> newVect(it->size());

    for (int i = 0; i < newVect.size(); ++i) {
      newVect[i] = new DBValue(*(it->at(i)));
    }

    externalL.push_back(newVect);
  }

  return externalL;
}
