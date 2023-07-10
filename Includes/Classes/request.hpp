#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string> 

//# include "constantes.hpp"
# include "server.hpp"
# include "utils.hpp"
// # include "response.hpp"
class Response;

class Request {

	public:

		// DEBUG
		void print();
		
		Request();
		~Request();
		Request(const Request &copy);
		Request &operator=(const Request &other);

		// Constructor
		Request(int socketFd, std::string method, std::string path, std::string version, std::vector<Server> servers);

		// Getters
		int			getSocketFd();
		std::string	getPath();
		std::string	getMethod();
		std::string	getQuery();
		// Server		*getServerConfig();
		Location 	getLocationConfig();

		// Setters
		void		setSocketFd(int socketFd);
		void		setPath(const std::string &path);
		void		setMethod(const std::string &method);
		// void		setConfig(Server *config);
		void		setConfig(Location config);
		void		setQuery(const std::string &query);

		// Check function
		bool		isAcceptedMethod();
		bool		isValidVersion();
		bool		isValidPath();

		//
		static Request parseRequest(std::string request, int fd, std::vector<Server> servers);
		// void generateResponse();
		// void sendResponse(int sockfd);

		// Print
		void		printConfig();

	private:
		int			_socketFd; //-> So we can send corresponding response on correct connection
		std::string	_method; // GET
		std::string _path; // /
		std::string	_version; // http 1.1
		std::string	_query; //
		
		Location	_config;


		std::string _server_name; // Maybe used for POST and DELETE, specifies the host name (server name)
		std::string _body; // used for POST method ?
		// Response 	*_response; // NEEDS TO BE POINTER BECAUSE OF INCLUDE CIRCLE

		std::string	_header;

		std::string	_somesheets;
		
		void getQueryFromPath();
};

#endif