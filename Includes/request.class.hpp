#ifndef REQUEST_CLASS_HPP
# define REQUEST_CLASS_HPP

# include "webserv.hpp"

# include "Constantes.hpp"

class Request {

	public:
		Request();
		~Request();
		Request(const Request &copy);
		Request &operator=(const Request &other);

		// Constructor
		Request(int socketFd, std::string path, std::string method, Server config);

		// Getters
		int getSocketFd();
		std::string getPath();
		std::string getMethod();
		Server getServerConfig();

		// Setters
		void		setSocketFd(int socketFd);
		void		setPath(const std::string &path);
		void		setMethod(const std::string &method);
		void		setConfig(const Server &config);
		
	private:
		int			_socketFd; //-> So we can send corresponding response on correct connection
		std::string _path;
		std::string	_method;
		Server 		_config;

		std::string	_somesheets;

};

#endif