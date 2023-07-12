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
	std::cout << "Server name : " << _server_name << std::endl;
	std::cout << "Body : " << _body << std::endl;
}


Request::Request()
{

}

Request::~Request()
{

}

Request::Request(const Request &copy) 
: _socketFd(copy._socketFd), _method(copy._method), _path(copy._path), _version(copy._version), _config(copy._config)
{}

Request &Request::operator=(const Request &other)
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
Request::Request(int socketFd, std::string method, std::string path, std::string version, Server server)
: _socketFd(socketFd), _method(method), _path(path), _version(version)
{
	
	std::cout << "On est dans le construct" << std::endl;
	std::cout << "path ? " << path << " NTM" << std::endl;
	_config = server.getLocationFromPath(path);
	
	std::cout << "Apres le config" << std::endl;


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

// Server *Request::getServerConfig()
// {
// 	return (_config);
// }
Location Request::getLocationConfig()
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

// void		Request::setConfig(Server *config)
// {
// 	this->_config = config;
// }
void		Request::setConfig(Location config)
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
	return _config.getMethod(_method);

	// isInVector(_config->getAcceptedMethods(), _method);
}

bool Request::isValidVersion()
{
	#ifdef DEBUG
	std::cout << "Version ? " << _version << std::endl;
	#endif

	return (_version == "HTTP/1.1" || _version == "HTTP/1.0");	
}

bool Request::isValidPath()
{
	std::string root =  _config.getRoot();

	std::cout << "ROOT => " << root << std::endl;
	std::cout << "Path => " << _path << std::endl;
	if (_path.compare(0, root.length(), root) == 0 || _path == "/")
		return true;
	return false;
}

Request Request::parseRequest(std::string request, int fd, Server server)
{
	std::cout << "Request: " << request << "\n\n\n\n" << std::endl;
	std::string method = "";
	std::string path = "";

	std::string::size_type pos = request.find(" ");
	method = request.substr(0, pos);
	std::string::size_type pos2 = request.find(" ", pos);
	path = request.substr(pos + 1, pos2 - 2);


	std::cout << "On crash ici ?" << std::endl;
	std::cout << "METHOD= " << method << std::endl;
	std::cout << "PATH= " << path << std::endl;


	Request res(fd, method, path, "HTTP/1.1", server);


	std::cout << "On crash ici ? after construct" << std::endl;

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
	// _config->print();
	std::cout << "\n\nPath: " << _path << "\nQuery: " << _query << std::endl;
}