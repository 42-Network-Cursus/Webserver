#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector> 
# include <string>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <fcntl.h>
# include <poll.h>

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

		// GETTERS
		const std::string 			&getPort() const;
		const std::string 			&getHost() const;
		const std::string 			&getServer_name() const;
		std::vector<struct pollfd>	&getPfds();
		std::vector<Location> 		&getLocation();
		Location 					&getLocationFromPath(std::string path);
		int 						getSockList() const;

		// SETTERS
		void setPfds(const std::vector<struct pollfd> &pfds);

		// DEBUG
		void print();

	private:
		std::string 				_port;
		std::string 				_host;
		std::string 				_server_name;
		std::vector<struct pollfd>	_pfds;
		std::vector<Location>		_locations;
		int 						_socklist; // Listening socket

};

#endif // SERVER_HPP