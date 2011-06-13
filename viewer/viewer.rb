require 'sinatra'
require 'rubygems'
require 'sqlite3'
require_relative 'loginHandler'

before do
	@db = SQLite3::Database.new "../feeds.db"
end

get '/' do
  require_auth	
  pair = [ session[:user], session[:user] ]
  @sources = @db.execute( "select sources.website_id, title, url, descr, c from sources LEFT JOIN (select website_id, COUNT(id) as c FROM posts WHERE read='0' AND user=? GROUP BY website_id) as posts ON sources.website_id = posts.website_id WHERE user=?", pair)
  haml :feedSources
end

get '/new/:feed' do
  require_auth	
  pair = [ params[:feed], session[:user] ]
  @posts = @db.execute("select title, link, description from posts where website_id=? and read='0' and user=? order by date desc", pair)
  @db.execute("update posts set read=1 where website_id=? and read=0 and user=?", pair)
  haml :posts
end  

get '/:feed' do
  require_auth	
  pair = [ params[:feed], session[:user] ]
  @posts = @db.execute( "select title, link, description from posts where website_id=? and user=? order by date desc limit 10", pair)
  @db.execute("update posts set read=1 where website_id=? and read=0 and user=?", pair)
  haml :posts
end

get '/:feed/:id' do
  
end

