#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string> 
# include <iostream>
# include <sstream>

# include "server.hpp"

class Request {

	public:
		Request();
		~Request();
		Request(const Request &copy);
		Request &operator=(const Request &other);

		// CONSTRUCTORS
		Request(int socketFd);
		Request(int socketFd, std::string method, std::string path, std::string version, Server server);
		Request(int socketFd, std::string method, std::string path, std::string version, Server server, std::string request);

		static Request parseRequest(std::string request, int fd, Server server);

		// GETTERS
		int			getSocketFd();
		std::string	getPath();
		std::string	getMethod();
		std::string	getQuery();
		Location 	&getLocationConfig();
		std::string getContentType();
		std::string getBody();
		int			getContentSize();
		std::string	getUploadPath();
		std::string	getRequest();
		std::string getVersion();
		std::string getServerName();
		std::string getDefaultPage();
		std::string getState();
		std::string getHeader();
		std::string getDefaultErrorPage();

		// SETTERS
		void		setSocketFd(int socketFd);
		void		setPath(const std::string &path);
		void		setMethod(const std::string &method);
		void		setConfig(Location config);
		void		setQuery(const std::string &query);
		void		setBody(const std::string &body);
		void		setContentSize(int size);
		void		setContentType(const std::string &type);
		void		setState(std::string state);

		// Check function
		bool		isAcceptedMethod();
		bool		isValidVersion();
		int			isValidPath();
		

		void	appendHeader(std::string data);
		void	appendBody(std::string data);
		int		getBodySize();

		// DEBUG
		void print();


	private:
		int			_socketFd; //-> So we can send corresponding response on correct connection
		std::string	_method;
		std::string _path;
		std::string	_version; // http 1.1
		Location	_config;
		std::string	_query;
		std::string _contentType;
		std::string _body;
		int 		_contentSize;
		std::string _serverName;
		std::string	_state;
		std::string _header;
		
		void getQueryFromPath();
		void getExtraDatas(std::string request);
		void checkMultiPart();
};

#endif