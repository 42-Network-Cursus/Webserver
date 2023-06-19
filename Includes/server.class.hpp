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
			next = rhs.next;
			client_max_body_size = rhs.client_max_body_size;

			sockfd = rhs.sockfd;
			list_sock = rhs.list_sock;

		}

		Server&	operator= (const Server& rhs) {
			if (this != &rhs) 
			{
				port = rhs.port;
				server_name = rhs.server_name;
				root = rhs.root;
				index = rhs.index;
				next = rhs.next;
				client_max_body_size = rhs.client_max_body_size;

				sockfd = rhs.sockfd;
				list_sock = rhs.list_sock;
			}
			return (*this);
		}

		

		

		// Debugging
		void print();
	
	// private:	
		// Getters & setters + private variables ?
		int list_sock;
		int sockfd;
		// struct pollfd pfd;
		std::string port;
		std::string server_name;
		std::string root;
		std::string index;
		std::string client_max_body_size;
		Server *next;
};



#endif // SERVER_CLASS_HPP