#ifndef SERVER_CLASS_HPP
# define SERVER_CLASS_HPP

# include "std_libs.hpp"


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


class Server {
	public:
		Server();

		Server(const Server &rhs) {
			std::cout << "COPY CONSTRUCTOR CALLED\n";
			port = rhs.port;
			server_name = rhs.server_name;
			root = rhs.root;
			index = rhs.index;
			// next = rhs.next;
			client_max_body_size = rhs.client_max_body_size;

			// sockfd = rhs.sockfd;
			socklist = rhs.socklist;

		}

		Server&	operator= (const Server& rhs) {
			std::cout << "Assign CALLED\n";
			if (this != &rhs) 
			{
				port = rhs.port;
				server_name = rhs.server_name;
				root = rhs.root;
				index = rhs.index;
				// next = rhs.next;
				client_max_body_size = rhs.client_max_body_size;

				// sockfd = rhs.sockfd;
				socklist = rhs.socklist;
			}
			return (*this);
		}

		void get_listening_socket();

		std::vector<std::string> getAcceptedMethods();
		std::string		getRoot();

		// Debugging
		void print();
	
	// private:	
		// Getters & setters + private variables ?

		// Sockets
		int socklist; // Listening socket, used by accept()
		std::vector<struct pollfd> pfds;
		
		// int sockfd;
		
		// Configuration parameters
		std::string port;
		std::string server_name;
		std::string root;
		std::string index;
		std::string client_max_body_size;
		
		std::vector<std::string> _methods;
		
		// Server *next;
};

#endif // SERVER_CLASS_HPP