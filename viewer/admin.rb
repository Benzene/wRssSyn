#!/usr/bin/env ruby
require 'rubygems'
require 'pg'
require 'irb'

@db = PG::Connection.open(:dbname => 'wrsssyn', :user => 'wrsssyn')

def run_req(sql)
	return @db.exec(sql).values
end

IRB.start(__FILE__)
