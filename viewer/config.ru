require 'sinatra'

use Rack::Lint
use Rack::ShowExceptions
use Rack::CommonLogger
use Rack::ContentLength

set :env, :production
set :port, 4568
disable :run, :reload

require_relative 'viewer'
