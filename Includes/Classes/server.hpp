#ifndef SERVER_HPP
# define SERVER_HPP

# include "std_libs.hpp"
# include "location.hpp"

class Server {
	public:

		Server();
		~Server();
		Server(const Server &copy);
		Server &operator=(const Server &other);


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
		int 						getSockList() const;

		// char * getLocation();
		void setPfds(const std::vector<struct pollfd> &pfds);
	private:
		std::string 				_port;
		std::string 				_host;
		std::string 				_server_name;
		std::vector<struct pollfd>	_pfds;
		std::vector<Location>		_locations;
		int 						_socklist; // Listening socket



		// DELETE THIS ONE, NOT USED ANYMORE
		int 						_sockfd; // Communication socket 


};

#endif // SERVER_HPP