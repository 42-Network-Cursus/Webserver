#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string> 

//# include "constantes.hpp"
# include "server.hpp"
# include "utils.hpp"

class Request {

	public:
		Request();
		~Request();
		Request(const Request &copy);
		Request &operator=(const Request &other);

		// Constructor
		Request(int socketFd, std::string method, std::string path, std::string version, Server *config);

		// Getters
		int			getSocketFd();
		std::string	getPath();
		std::string	getMethod();
		std::string	getQuery();
		Server		*getServerConfig();

		// Setters
		void		setSocketFd(int socketFd);
		void		setPath(const std::string &path);
		void		setMethod(const std::string &method);
		void		setConfig(Server *config);
		void		setQuery(const std::string &query);

		// Check function
		bool		isAcceptedMethod();
		bool		isValidVersion();
		bool		isValidPath();

		//
		static Request parseRequest(std::string request, int fd, std::vector<Server *> servers);

		// Print
		void		printConfig();

	private:
		int			_socketFd; //-> So we can send corresponding response on correct connection
		std::string	_method;
		std::string _path;
		std::string	_version;
		std::string	_query;
		Server		*_config;

		//std::string	_method;
		std::string	_header;

		std::string	_somesheets;
		
		void getQueryFromPath();
};

#endif