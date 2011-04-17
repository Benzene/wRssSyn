require 'sinatra'
require 'rubygems'
require 'sqlite3'

before do
	@db = SQLite3::Database.new "../feeds.db"
end

get '/' do
  @sources = @db.execute ( "select * from sources LEFT JOIN (select website_id, COUNT(id) as c FROM posts WHERE read='0' GROUP BY website_id) as posts ON sources.website_id = posts.website_id")
  haml :feedSources
end

get '/new/:feed' do
  pair = [ params[:feed] ]
  @posts = @db.execute ("select * from posts where website_id=? and read='0' order by date desc"), pair
  @db.execute ("update posts set read=1 where website_id=? and read=0"), pair
  haml :posts
end  

get '/:feed' do
  pair = [ params[:feed] ]
  @posts = @db.execute ( "select * from posts where website_id=? order by date desc limit 10"), pair
  @db.execute ("update posts set read=1 where website_id=? and read=0"), pair
  haml :posts
end

get '/:feed/:id' do
  
end

