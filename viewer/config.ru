require 'sinatra'

use Rack::Lint
use Rack::ContentLength

require './viewer'
run Sinatra::Application
