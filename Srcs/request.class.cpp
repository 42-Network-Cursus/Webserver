#include "request.class.hpp"

Request::Request()
{

}

Request::~Request()
{

}

Request::Request(const Request &copy)
{

}
Request& Request::operator=(const Request &other)
{

}


// Constructor
Request::Request(int socketFd, std::string path, std::string method, Server config)
: _socketFd(socketFd), _path(path), _method(method), _config(config)
{

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

Server Request::getServerConfig()
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
void		Request::setConfig(const Server &config)
{
	this->_config = config;
}