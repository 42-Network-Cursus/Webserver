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
		Request(int socketFd, std::string method, std::string path, std::string version, Server server);
		Request(int socketFd, std::string method, std::string path, std::string version, Server server, std::string request);

		// Getters
		int			getSocketFd();
		std::string	getPath();
		std::string	getMethod();
		std::string	getQuery();
		// Server		*getServerConfig();
		Location 	getLocationConfig();
		std::string getContentType();
		std::string getBody();
		int			getContentSize();
		std::string getUploadPath();

		// Setters
		void		setSocketFd(int socketFd);
		void		setPath(const std::string &path);
		void		setMethod(const std::string &method);
		// void		setConfig(Server *config);
		void		setConfig(Location config);
		void		setQuery(const std::string &query);
		void		setBody(const std::string &body);
		void		setContentSize(int size);

		// Check function
		bool		isAcceptedMethod();
		bool		isValidVersion();
		bool		isValidPath();

		//
		static Request parseRequest(std::string request, int fd, Server server);
		// void generateResponse();
		// void sendResponse(int sockfd);

		std::string getDefaultPage();
		// Print
		void		printConfig();

	private:
		int			_socketFd; //-> So we can send corresponding response on correct connection
		std::string	_method; // GET
		std::string _path; // /
		std::string	_version; // http 1.1
		std::string	_query; //
		std::string _contentType;
		std::string _body;
		int _contentSize;

		Location	_config;


		std::string _server_name; // Maybe used for POST and DELETE, specifies the host name (server name)
		 // used for POST method ?
		// Response 	*_response; // NEEDS TO BE POINTER BECAUSE OF INCLUDE CIRCLE

		std::string	_header;

		std::string	_somesheets;
		
		void getQueryFromPath();
		void getExtraDatas(std::string request);
		void checkMultiPart();
};

#endif