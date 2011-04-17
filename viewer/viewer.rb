require 'sinatra'
require 'rubygems'
require 'sqlite3'

before do
	@db = SQLite3::Database.new "../feeds.db"
end

get '/' do
  @sources = @db.execute ( "select * from sources")
  haml :feedSources
end


get '/:feed' do
  pair = [ params[:feed] ]
  @posts = @db.execute ( "select * from posts where website_id=? order by date desc limit 10"), pair
  haml :posts
end

get '/:feed/:id' do
  
end

