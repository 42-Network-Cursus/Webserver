#include "request.hpp"

Request::Request()
{

}

Request::~Request()
{

}

Request::Request(const Request &copy) : _socketFd(copy._socketFd), _method(copy._method), _path(copy._path), _version(copy._version), _config(copy._config)
{

}

Request& Request::operator=(const Request &other)
{
	if (this != &other)
	{
		this->_socketFd = other._socketFd;
		this->_method = other._method;
		this->_path = other._path;
		this->_version = other._version;
		this->_config = other._config;
	}
	return (*this);
}


// Constructor
Request::Request(int socketFd, std::string method, std::string path, std::string version, Server *config)
: _socketFd(socketFd), _method(method), _path(path), _version(version), _config(config)
{
	getQueryFromPath();
}


// Getters
int Request::getSocketFd()
{
	return (_socketFd);
}

std::string Request::getPath()
{
	return (_path);
}

std::string Request::getMethod()
{
	return (_method);
}

std::string Request::getQuery()
{
	return (_query);
}

Server *Request::getServerConfig()
{
	return (_config);
}

// Setters

void		Request::setSocketFd(int socketFd)
{
	this->_socketFd = socketFd;
}

void		Request::setPath(const std::string &path)
{
	this->_path = path;
}

void		Request::setMethod(const std::string &method)
{
	this->_method = method;
}

void		Request::setConfig(Server *config)
{
	this->_config = config;
}

void	Request::setQuery(const std::string &query)
{
	this->_query = query;
}

// Method

bool Request::isAcceptedMethod()
{
	return isInVector(_config->getAcceptedMethods(), _method);
}

bool Request::isValidVersion()
{
	std::cout << "Version ? " << _version << std::endl;
	return (_version == "HTTP/1.1" || _version == "HTTP/1.0");	
}

bool Request::isValidPath()
{
	std::string root = _config->getRoot();

	std::cout << "ROOT => " << root << std::endl;
	std::cout << "Path => " << _path << std::endl;
	if (_path.compare(0, root.length(), root) == 0 || _path == "/")
		return true;
	return false;
}

Request Request::parseRequest(std::string request, int fd, std::vector<Server *> servers)
{

	std::cout << "PARSE REQUEST SIZE " << servers.size() << std::endl;
	std::cout << "PARSE REQUEST PRINT\n\n" << std::endl;
	servers[0]->print();

	std::string method = "";
	std::string path = "";

	std::string::size_type pos = request.find(" ");
	method = request.substr(0, pos);
	std::string::size_type pos2 = request.find(" ", pos);
	path = request.substr(pos + 1, pos2 - 2);

	std::cout << "\nMethod " << method << "\nPath " << path << "\n\n\n" << std::endl;

// int socketFd, std::string path, std::string method, Server config
	Request res = Request(fd, method, path, "HTTP/1.1", servers[0]);

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
		_path = _path.substr(pos);
	}
}

// Print

void Request::printConfig()
{
	_config->print();
	std::cout << "\n\nPath: " << _path << "\nQuery: " << _query << std::endl;
}