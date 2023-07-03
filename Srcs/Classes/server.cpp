#include "webserv.hpp"

// Debugging
void Server::print() {
	std::cout << "port: " << port << std::endl;
	std::cout << "server name: " << server_name << std::endl;
}

// Default constructor
Server::Server() 
{}

/*
	int getaddrinfo(
		const char *node, -> host name / IP address to connect to (ex: www.ex.com / IP) IF NULL, fills hints with our IP
		const char *service, -> port number
		const struct addrinfo *hints, -> prob no need, send an empty one
		struct addrinfo **res -> fills your given pointer with result
	)

	struct addrinfo {
               int              ai_flags;
               int              ai_family;
               int              ai_socktype;
               int              ai_protocol;
               socklen_t        ai_addrlen;
               struct sockaddr *ai_addr;
               char            *ai_canonname;
               struct addrinfo *ai_next;
           };
*/

void Server::get_listening_socket() {
	struct addrinfo hints, *ai, *p;
	int yes = 1;
	int rv;

	memset(&hints, 0, sizeof(hints)); // To make sure it is empty
	hints.ai_family = AF_UNSPEC; // either IPV4 or 6, no need to specify
	hints.ai_socktype = SOCK_STREAM; // TCP stream socket
	
/********/
	// MODIF: Potentially in if/else statement, depending on wether we received a server name in configuration
	hints.ai_flags = AI_PASSIVE; // use my IP
/*******/
	
	// TO DELETE
	// if (server_name == "blabla")
	// 	server_name = "localhost";

	// if ((rv = getaddrinfo(server_name.c_str(), port.c_str(), &hints, &ai)) != 0) {
	if ((rv = getaddrinfo("127.0.0.1", port.c_str(), &hints, &ai)) != 0) {
		std::cerr << "getaddrinfo error " << rv << ": " << gai_strerror(rv) << std::endl;
		exit(1);
	}

	for (p = ai; p != NULL; p = p->ai_next) {
		if ((socklist = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
			continue;

		// If "address already in use" is a problem :
		setsockopt(socklist, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
		
		// Makes the socket non-blocking.
		fcntl(socklist, F_SETFL, O_NONBLOCK);

		if (bind(socklist, p->ai_addr, p->ai_addrlen) < 0) {
			close(socklist);
			continue;
		}

		break;
	}

	if (p == NULL) {
		// std::cerr << "listen: " << strerror(errno) << std::endl;
		std::cerr << "Could not find address to bind to" << std::endl;
		exit(1);
	}

	freeaddrinfo(ai); // All done with this structure

	if (listen(socklist, BACKLOG) == -1) {
		std::cerr << "listen: " << strerror(errno) << std::endl;
		exit(1);
	}

	// TO DELETE
	std::cout << "FOUND LISTENER FOR RANDOM HOSTNAME\n"; 

	struct pollfd pfd;
	
	pfd.fd = socklist;
	pfd.events = POLLIN;
	pfds.push_back(pfd);
}


void Server::store_server_configuration(std::ifstream &file_stream) {

	std::string	line;

	while (1) {
		std::getline(file_stream, line);

		line = trim(line);
		if (skip_line(line))
			continue;
		if (line == "}")
			break;

		std::string param = line.substr(0, line.find_first_of(" "));

		if (param == "location") {
			// store location
		}
		else {

			std::string param_val = line.substr(line.find_first_of(" "), line.find_first_of(";") - line.find_first_of(" "));
			param_val = trim(param_val);
			
			if (param == "listen") {
				host = param_val.substr(0, param_val.find_first_of(":"));
				port = param_val.substr(param_val.find_first_of(":") + 1, param_val.find_first_of(";") - param_val.find_first_of(":"));
			}
			else if (param == "server_name") {
				server_name = param_val;
			}
			
		}
	} // End While

}

/*
	*** GETTERS ***
*/

const char *Server::getPort() const {
	return port.c_str();
}

const char *Server::getHost() const {
return host.c_str();
}

const std::string Server::getserver_name() const {
return port;
}
