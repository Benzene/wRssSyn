require 'sinatra'
require 'sqlite3'
require 'bcrypt'

enable :inline_templates

class AuthenticationError < StandardError; end
class ParameterError < StandardError; end

use Rack::Session::Pool, :expire => 60 * 60 * 24

error AuthenticationError do
	"Authentication failed !"
	redirect url('/login')
end

error ParameterError do
	"Wrong parameters given !"
end

helpers do
	def is_authed?
		!session[:user].nil?
	end
	def auth(user, pass)
		raise AuthenticationError, 'No credentials given' unless (user.is_a?(String) && pass.is_a?(String))
		pair = [ user ]
		res = @auth_db.execute("SELECT hashed_pass FROM users WHERE username=?", pair)
		raise AuthenticationError, 'Invalid credentials (username not found)' unless res.length > 0
		raise AuthenticationError, 'Invalid credentials (wrong password)' unless BCrypt::Password.new(res[0][0]) == pass
		session[:user] = user
	end
	def require_auth
		if !is_authed? then
			redirect url('/login')
		end
	end
	def require_non_auth
		if is_authed? then
			redirect url('/')
		end
	end
end

before do
	@auth_db = SQLite3::Database.new "users.db"
end

# An usage example :
#get '/' do
#	require_auth
#	pair = [ session[:user] ]
#end

get '/login' do
	require_non_auth
	haml :login
end

post '/login' do
	require_non_auth
	auth(params[:user], params[:pass])
	is_authed?
	redirect url('/')
end

get '/register' do
	require_non_auth
	haml :register
end

post '/register' do
	require_non_auth
	pair = [ params[:user], BCrypt::Password.create(params[:pass]), params[:email] ]
	@auth_db.execute("INSERT INTO users (username,hashed_pass,email) VALUES(?,?,?)", pair)
	redirect url('/')
end

__END__

@@ login
%form{:action => url('/login'), :method => "post"}
  User :
  %input{:type => "text", :name => "user"}
  %br
  Password :
  %input{:type => "password", :name => "pass"}
  %br
  %input{:type => "submit"}

@@ register
%form{:action => url('/register'), :method => "post"}
  User :
  %input{:type => "text", :name => "user"}
  %br
  Email :
  %input{:type => "text", :name => "email"}
  %br
  Password :
  %input{:type => "password", :name => "pass"}
  %br
  %input{:type => "submit"}

