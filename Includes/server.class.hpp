#ifndef SERVER_CLASS_HPP
# define SERVER_CLASS_HPP

# include <iostream>

class Server {
	public:
		Server();

		Server(const Server &rhs) {
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
		
		// Server *next;
};



#endif // SERVER_CLASS_HPP