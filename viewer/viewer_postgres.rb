require 'sinatra'
require 'rubygems'
require 'pg'
require_relative 'login-commons/loginHandler'

before do
	@db = PGconn.open(:dbname => 'wrsssyn', :user => 'wrsssyn')
end

get '/' do
  require_auth	
  pair = [ session[:user] ]
  @sources = @db.exec('SELECT sources.website_id, title, url, descr, c FROM sources LEFT JOIN (SELECT website_id, COUNT(uid) as c FROM posts WHERE posts.uid NOT IN (SELECT post_id FROM readeds WHERE "user"=$1) GROUP BY website_id) as counts ON sources.website_id = counts.website_id INNER JOIN (SELECT sources_id FROM subscriptions WHERE "user"=$1) AS subscriptions ON sources.website_id=subscriptions.sources_id', pair)
  haml :feedSources
end

get '/new/:feed' do
  require_auth	
  pair = [ params[:feed], session[:user] ]
  @posts = @db.exec('SELECT uid, title, link, description FROM posts INNER JOIN (SELECT sources_id as src FROM subscriptions WHERE "user"=$2) as c2 ON posts.website_id=c2.src WHERE posts.website_id=$1 AND posts.uid NOT IN (SELECT post_id FROM readeds WHERE "user"=$2) ORDER BY posts.date DESC', pair)

  # To be improved.
  @posts.each { |post|
	  pair = [ post['uid'], session[:user] ]
	  @db.exec('INSERT INTO readeds ("post_id","user") VALUES ($1,$2)', pair) 
  }
  haml :posts
end  

get '/:feed' do
  require_auth	
  pair = [ params[:feed], session[:user] ]
  @posts = @db.exec('SELECT uid, title, link, description FROM posts INNER JOIN (SELECT sources_id as src FROM subscriptions WHERE "user"=$2) as c2 ON posts.website_id=c2.src WHERE posts.website_id=$1 ORDER BY posts.date DESC LIMIT 10', pair)

  haml :posts
end

get '/:feed/:id' do
  
end

