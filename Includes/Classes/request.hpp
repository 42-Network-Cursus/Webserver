#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string> 
# include <iostream>
# include <sstream>

# include "server.hpp"
# include "utils.hpp"

class Request {

	public:
		Request();
		~Request();
		Request(const Request &copy);
		Request &operator=(const Request &other);

		// CONSTRUCTORS
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

		// SETTERS
		void		setSocketFd(int socketFd);
		void		setPath(const std::string &path);
		void		setMethod(const std::string &method);
		void		setConfig(Location config);
		void		setQuery(const std::string &query);
		void		setBody(const std::string &body);
		void		setContentSize(int size);
		void		setContentType(const std::string &type);

		// Check function
		bool		isAcceptedMethod();
		bool		isValidVersion();
		int			isValidPath();
		

		std::string getDefaultPage();
		
		// DEBUG
		void		printConfig();
		void print();


	private:
		int			_socketFd; //-> So we can send corresponding response on correct connection
		std::string	_method; // GET
		std::string _path; // 
		std::string	_version; // http 1.1
		Location	_config;
		std::string	_query;
		std::string _contentType;
		std::string _body;
		int 		_contentSize;

		std::string _server_name; // Maybe used for POST and DELETE, specifies the host name (server name)
		
		void getQueryFromPath();
		void getExtraDatas(std::string request);
		void checkMultiPart();
};

#endif