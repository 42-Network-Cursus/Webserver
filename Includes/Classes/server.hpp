#ifndef SERVER_HPP
# define SERVER_HPP

# include "std_libs.hpp"
# include "location.hpp"

class Server {
	public:

		// Server();
		// ~Server();
		// Server(const Server &rhs);
		// Server operator=(const Server &rhs);


		void 		store_server_configuration(std::ifstream &file_stream);
		Location	get_location_config(std::ifstream &file_stream, std::string line);
		void 		get_listening_socket();

		// Debugging
		void print();
	

		// Setters 
		void setSockFD(int fd);

		// Getters
		const std::string 			&getPort() const;
		const std::string 			&getHost() const;
		const std::string 			&getServer_name() const;
		std::vector<struct pollfd>	&getPfds();
		std::vector<Location> 		&getLocation();
		Location 					&getLocationFromPath(std::string path);
		int 						getSockFD() const;

		// char * getLocation();
		void setPfds(const std::vector<struct pollfd> &pfds);
	private:
		std::string 				_port;
		std::string 				_host;
		std::string 				_server_name;
		std::vector<struct pollfd>	_pfds;
		std::vector<Location>		_locations;


		// Sockets
		int 						_socklist; // Listening socket
		int 						_sockfd; // Communication socket
		
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