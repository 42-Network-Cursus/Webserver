#include "webserv.hpp"
#include "utils.hpp"

// Debugging
void Server::print() {
	std::cout << "port: " << _port << std::endl;
	std::cout << "host: "<< _host << std::endl;
	std::cout << "server name: " << _server_name << std::endl;
	std::cout << "pfds size: " << _pfds.size() << std::endl;
	std::cout << "Locations: " << std::endl;
	std::vector<Location>::iterator loc_begin = _locations.begin();
	std::vector<Location>::iterator loc_end = _locations.end();
	for (; loc_begin != loc_end; loc_begin++) {
		loc_begin->print();
		std::cout << std::endl;
	}
}

Server::Server()
{}

Server::~Server() 
{}

Server::Server(const Server &copy) 
: _port(copy._port), _host(copy._host), _server_name(copy._server_name), _pfds(copy._pfds), _locations(copy._locations), _socklist(copy._socklist)
{}

Server &Server::operator=(const Server &other) {
	
	if (this != &other)
	{
		_port = other._port;
		_host = other._host;
		_server_name = other._server_name;
		_pfds = other._pfds;
		_locations = other._locations;
		_socklist = other._socklist;
	}
	return *this;
}

void Server::get_listening_socket() {
	
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

	struct addrinfo hints, *ai, *p;
	int yes = 1;
	int rv;

	memset(&hints, 0, sizeof(hints)); // To make sure it is empty
	hints.ai_family = AF_UNSPEC; // either IPV4 or 6, no need to specify
	hints.ai_socktype = SOCK_STREAM; // TCP stream socket
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(_host.c_str(), _port.c_str(), &hints, &ai)) != 0) {
		std::cerr << "getaddrinfo error " << rv << ": " << gai_strerror(rv) << std::endl;
		G_EXIT = true;
	}

	for (p = ai; p != NULL; p = p->ai_next) {
		if ((_socklist = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
			continue;

		// If "address already in use" is a problem :
		setsockopt(_socklist, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
		
		// Makes the socket non-blocking.
		fcntl(_socklist, F_SETFL, O_NONBLOCK);

		if (bind(_socklist, p->ai_addr, p->ai_addrlen) < 0) {
			close(_socklist);
			continue;
		}

		break;
	}

	if (p == NULL) {
		std::cerr << "Could not find address to bind to" << std::endl;
		G_EXIT = true;
	}

	freeaddrinfo(ai); // All done with this structure

	if (listen(_socklist, BACKLOG) == -1) {
		std::cerr << "listen: " << strerror(errno) << std::endl;
		G_EXIT = true;
	} 

	struct pollfd pfd;
	
	pfd.fd = _socklist;
	pfd.events = POLLIN;
	_pfds.push_back(pfd);
}

Location Server::get_location_config(std::ifstream &file_stream, std::string line) {
	Location rv;

	line.erase(line.end() - 1);
	line.erase(0,8); // Deletes characters "location"

	line = trim(line);

	rv.setPath(line);
	
	while (1) {

		std::getline(file_stream, line);

		line = trim(line);
		if (skip_line(line))
			continue;
		if (line == "]")
			break;

		std::string param = line.substr(0, line.find_first_of(" "));
		std::string param_val = line.substr(line.find_first_of(" "), line.find_first_of(";") - line.find_first_of(" "));
		
		if (param == "root")
			rv.setRoot(param_val);
		else if (param == "index")
			rv.setIndex(param_val);
		else if (param == "client_max_body_size")
			rv.setClientMaxBodySize(param_val);
		else if (param == "error_page")
			rv.setErrorPagePath(param_val);
		else if (param == "script_path")
			rv.setScriptPath(rv.getRoot() + trim(param_val));
		else if (param == "upload_path")
			rv.setUploadPath(param_val);
		else if (param == "auto_index")
			rv.setAutoIndex(param_val);
		else if (param == "GET")
			rv.setMethod("GET", stringToBool(param_val));
		else if (param == "POST")
			rv.setMethod("POST", stringToBool(param_val));
		else if (param == "DELETE")
			rv.setMethod("DELETE", stringToBool(param_val));
	}
	rv.setHostPort(trim(_host) + ":" + trim(_port));
	return rv;
}

void Server::close_fds() {
	std::vector<struct pollfd>::iterator it_begin = _pfds.begin();
	std::vector<struct pollfd>::iterator it_end = _pfds.end();

	for (; it_begin != it_end; it_begin++)
		close(it_begin->fd);
}
void Server::store_server_configuration(std::ifstream &file_stream) {

	std::string	line;

	while (1) {
		std::string param = "";
		std::getline(file_stream, line);

		line = trim(line);
		if (skip_line(line))
			continue;
		if (line == "}")
			break;

		param = line.substr(0, line.find_first_of(" "));

		if (param == "location")
			_locations.push_back(get_location_config(file_stream, line));
		else {
			std::string param_val = line.substr(line.find_first_of(" "), line.find_first_of(";") - line.find_first_of(" "));
			param_val = trim(param_val);
			
			if (param == "listen") {
				_host = param_val.substr(0, param_val.find_first_of(":"));
				_port = param_val.substr(param_val.find_first_of(":") + 1, param_val.find_first_of(";") - param_val.find_first_of(":"));
			}
			else if (param == "server_name") {
				_server_name = param_val;
			}
		}
	} // End While
}

/*
	*** GETTERS ***
*/

const std::string 			&Server::getPort() const { return _port; }

const std::string 			&Server::getHost() const { return _host; }

const std::string 			&Server::getServer_name() const { return _server_name; }

std::vector<struct pollfd>	&Server::getPfds() { return _pfds; }

std::vector<Location> 		&Server::getLocation() { return _locations; }

int 						Server::getSockList() const { return _socklist; }

Location 					&Server::getLocationFromPath(std::string path) {

	std::vector<Location>::iterator it_start = _locations.begin();
	std::vector<Location>::iterator it_end = _locations.end();

	std::string tmp;
	path = trim(path);
	while (it_start != it_end)
	{
		tmp = it_start->getPath();
		if (path.compare(0, tmp.length(), tmp) == 0 && tmp != "/" && path != "/")
			break;
		if (path == "/")
			break ;
		++it_start;
	}

	if (it_start == it_end)
		it_start = _locations.begin();
	return *it_start;
}

/*
	*** SETTERS ***
*/

void Server::setPfds(const std::vector<struct pollfd> &pfds) { _pfds = pfds; }