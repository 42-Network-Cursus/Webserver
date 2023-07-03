#ifndef SERVER_HPP
# define SERVER_HPP

# include "std_libs.hpp"
# include "location.hpp"

class Server {
	public:
		Server();

		Server(const Server &rhs) {
			port = rhs.port;
			server_name = rhs.server_name;
			

			// sockfd = rhs.sockfd;
			socklist = rhs.socklist;

		}

		Server&	operator= (const Server& rhs) {
			if (this != &rhs) 
			{
				port = rhs.port;
				server_name = rhs.server_name;
				

				// sockfd = rhs.sockfd;
				socklist = rhs.socklist;
			}
			return (*this);
		}

		void store_server_configuration(std::ifstream &file_stream);
		void get_listening_socket();

		// Debugging
		void print();
	
		// Getters
		const char *getPort() const;
		const char *getHost() const;
		const std::string getserver_name() const;
		// char * getLocation();
		
	private:
		std::string port;
		std::string host;
		std::string server_name;
	
		std::vector<Location> locations;


		// Sockets
		int socklist; // Listening socket, used by accept()   Used ?	
		std::vector<struct pollfd> pfds;
		
		// Configuration parameters

		
		// std::string root;
		// std::string index;
		// std::string client_max_body_size;

};

#endif // SERVER_HPP








// std::string Configuration::getBody(int fd) {
// 	std::vector<Server>::iterator it_begin = _server_list.begin();
// 	std::vector<Server>::iterator it_end = _server_list.end();

// 	for (; it_begin != it_end; it_begin++) {
// 		if (it_begin->sockfd == fd)
// 			break;
// 	}
	
// 	std::string body;
// 	std::ifstream file_stream (it_begin->root + it_begin->index);

// 	if (!file_stream.is_open()) { // check whether the file is open
// 		std::cout << "Error reading conf file" << std::endl;
// 		exit(1);
// 	}
	
// 	body.assign ( 	(std::istreambuf_iterator<char>(file_stream)),
// 					(std::istreambuf_iterator<char>()) 
// 				);

// 	return body;
// }