#include "request.hpp"
#include "response.hpp"

// DEBUG
void Request::print() {
	std::cout << "\n\nRequest info: " << std::endl;
	std::cout << "Socket : " << _socketFd << std::endl;
	std::cout << "Method : " << _method << std::endl;
	std::cout << "Path : " << _path << std::endl;
	std::cout << "Version : " << _version << std::endl;
	std::cout << "Query : " << _query << std::endl;
	std::cout << "\nConfig : " << std::endl;
	_config.print();
	std::cout << "Server name : " << _serverName << std::endl;
	std::cout << "Content-Size: " << _contentSize << std::endl;
	std::cout << "Content-Type: " << _contentType << std::endl;
	std::cout << "Body : " << _body << std::endl << std::endl;
}

void Request::printConfig()
{
	// _config->print();
	//std::cout << "\n\nPath: " << _path << "\nQuery: " << _query << "\nContent-Type: " << _contentType << std::endl;
}

// CONSTRUCTORS
Request::Request()
{
	_method = REQ_INV;
}

Request::~Request()
{}

Request::Request(const Request &copy) 
: _socketFd(copy._socketFd), _method(copy._method), _path(copy._path), _version(copy._version), _config(copy._config)
{
	this->_contentSize = copy._contentSize;
	this->_contentType = copy._contentType;
	this->_body = copy._body;
	this->_query = copy._query;
	this->_serverName = copy._serverName;
}

Request &Request::operator=(const Request &other)
{
	if (this != &other)
	{
		this->_socketFd = other._socketFd;
		this->_method = other._method;
		this->_path = other._path;
		this->_version = other._version;
		this->_config = other._config;
		this->_contentSize = other._contentSize;
		this->_contentType = other._contentType;
		this->_body = other._body;
		this->_query = other._query;
		this->_serverName = other._serverName;
		
	}
	return (*this);
}


// Constructor w/ request
Request::Request(int socketFd, std::string method, std::string path, std::string version, Server server)
: _socketFd(socketFd), _method(method), _path(path), _version(version)
{
	_config = server.getLocationFromPath(path);
	getQueryFromPath();
	_body = "";
	_contentSize = 0;
	_contentType = "";

	_serverName = server.getServer_name();
}

Request::Request(int socketFd, std::string method, std::string path, std::string version, Server server, std::string request)
: _socketFd(socketFd), _method(method), _path(path), _version(version)
{
	
	//substr from "?"
	_config = server.getLocationFromPath(path);
	getQueryFromPath();
	
	//std::cout << "\n\n ROOT **" << _config.getRoot() <<  "\n\n\n";

	getExtraDatas(request);
	checkMultiPart();
	_body = "";
	_contentSize = 0;
	
	_serverName = server.getServer_name();
}

// GETTERS
int 		Request::getSocketFd() 			{ return (_socketFd); }

std::string Request::getPath()				{ return (_path); }

std::string Request::getMethod()			{ return (_method); }

std::string Request::getQuery()				{ return (_query); }

Location 	&Request::getLocationConfig()	{ return (_config); }

std::string Request::getContentType() 		{ return (_contentType); }

std::string Request::getBody() 				{ return (_body); }

int 		Request::getContentSize() 		{ return (_contentSize); }

std::string Request::getRequest() 			{ return (_config.getRedirect()); }

std::string Request::getVersion() 			{ return _version; }

// SETTERS
void		Request::setSocketFd(int socketFd) 				{ this->_socketFd = socketFd; }

void		Request::setPath(const std::string &path) 		{ this->_path = path; }

void		Request::setMethod(const std::string &method)	{ this->_method = method; }

void		Request::setConfig(Location config) 			{ this->_config = config; }

void	Request::setQuery(const std::string &query) 		{ this->_query = query; }

void Request::setBody(const std::string &body) 				{ this->_body = body; }

void Request::setContentSize(int size) 						{ this->_contentSize = size; }

void Request::setContentType(const std::string &type) 		{ this->_contentType = type; }

// METHODS
bool Request::isAcceptedMethod() 	{ return _config.getMethod(_method); }

bool Request::isValidVersion()
{
	_version = trim(_version);
	return (_version == "HTTP/1.1" || _version == "HTTP/1.0");	
}

int Request::isValidPath()
{
	if (_path == _config.getPath())
	{
		if (_config.getAutoIndex())
			return 200;

		std::ifstream file;
		std::string root = _config.getRoot();
		std::string index = _config.getIndex();
		index = trim(index);
		std::string fullPath = root + index;
		fullPath = trim(fullPath);

		file.open(fullPath.c_str(), std::ifstream::in);
		if (file.is_open() == false)
		{
			index = _config.getRedirect();
			index = trim(index);
			fullPath = root + index;
			fullPath = trim(fullPath);

			file.open(fullPath.c_str(), std::ifstream::in);
			if (file.is_open() == false)
				return 404;
			file.close();
			_path = _config.getPath() + index;
			return 301;
		}
		file.close();
	}
	return 200;
}

std::string Request::getDefaultPage()
{
	std::string root = deleteWhiteSpace(_config.getRoot());
	std::string page = deleteWhiteSpace(_config.getIndex());

	return root + page;
}

Request Request::parseRequest(std::string request, int fd, Server server)
{
	std::string method = "";
	std::string path = "";
	std::string version = "";

	std::string::size_type pos = request.find(" ");
	std::string::size_type pos2 = request.find(" ", pos + 1);

	method = request.substr(0, pos);
	path = request.substr(pos + 1, pos2 - pos - 1);

	pos = request.find("HTTP/");
	pos2 = request.find("\n");
	version = request.substr(pos, pos2 - pos);
	version = trim(version);

	Request res(fd, method, path, version, server, request);
	return res;
}

void Request::getQueryFromPath()
{
	size_t pos = _path.find_first_of('?');
	if (pos == std::string::npos)
		_query = "";
	else
	{
		_query.assign(_path, pos + 1, std::string::npos);
		_path = _path.substr(0, pos);
	}
}



void Request::getExtraDatas(std::string request)
{
	std::istringstream iss(request);
	std::string line;

	while (std::getline(iss, line)) {
		if (line.compare(0, 13, "Content-Type:") == 0) {	
			_contentType = line.substr(14);
			return ;
		}
	}
}

std::string Request::getUploadPath()
{
	return _config.getUploadPath();
}

std::string Request::getServerName() 
{
	return _serverName;
}

void Request::checkMultiPart()
{
	size_t pos = _contentType.find(CT_MULTI);
	if (pos == std::string::npos)
		return ;
	
	setContentType(CT_MULTI);
}