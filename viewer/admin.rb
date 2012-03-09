#!/usr/bin/env ruby
require 'rubygems'
require 'pg'
require 'irb'

@db = PG::Connection.open(:dbname => 'wrsssyn', :user => 'wrsssyn')

def run_req(sql)
	return @db.exec(sql).values
end

def delete_last_post(website_id)
	r = @db.exec("SELECT uid FROM posts WHERE website_id='#{website_id}' ORDER BY uid DESC LIMIT 1")
	id = r[0]['uid']
	run_req("DELETE FROM readeds WHERE post_id='#{id}'")
	run_req("DELETE FROM posts WHERE uid='#{id}'")
end

IRB.start(__FILE__)
