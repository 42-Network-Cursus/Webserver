#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>

class Server {
	public:
		Server() {
			next = NULL;
			sockfd = -1;
			client_max_body_size = "0M";
		}

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

		// Getters & setters + private variables ?
		
		int list_sock;
		int sockfd;
		std::string port;
		std::string server_name;
		std::string root;
		std::string index;
		std::string client_max_body_size;
		Server *next;

		

		// Debugging
		void print() {
			std::cout << "port: " << port << std::endl;
			std::cout << "server name: " << server_name << std::endl;
			std::cout << "root: " << root << std::endl;
			std::cout << "index: " << index << std::endl;
			std::cout << "client_max_body_size: " << client_max_body_size << std::endl;
		}
	private:	
};



#endif // SERVER_HPP