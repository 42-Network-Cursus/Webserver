#include "server.hpp"

class Server {
	public:
		Server() {
			next = NULL;
		}

		Server(const Server &rhs) {
			port = rhs.port;
			server_name = rhs.server_name;
			root = rhs.root;
			index = rhs.index;
			next = rhs.next;
		}

		Server&	operator= (const Server& rhs) {
			if (this != &rhs) 
			{
				port = rhs.port;
				server_name = rhs.server_name;
				root = rhs.root;
				index = rhs.index;
				next = rhs.next;
			}
			return (*this);
		}

		// Getters & setters + private variables ?
		
		int sockfd;
		std::string port;
		std::string server_name;
		std::string root;
		std::string index;
		Server *next;

		

		// Debugging
		void print() {
			std::cout << "port: " << port << std::endl;
			std::cout << "server name: " << server_name << std::endl;
			std::cout << "root: " << root << std::endl;
			std::cout << "index: " << index << std::endl;
		}
	private:	
};

class Configuration {
	public:
		int size() const {
			return _server_list.size();
		}

		void push_back(const Server& val) { // const Server ?
			_server_list.push_back(val);
		}

		Server&	operator[] (int idx) { 
			return _server_list[idx];
		}

		// // Debugging
		void print() {
			std::vector<Server>::iterator it_begin = _server_list.begin();
			std::vector<Server>::iterator it_end = _server_list.end();

			std::cout << "Number of servers: " << _server_list.size() << std::endl << std::endl;
			for (; it_begin != it_end; it_begin++) {
				it_begin->print();
				std::cout << std::endl;
			}
		}

	private:
		std::vector<Server>	_server_list;		
};

enum conf_param {
	port,
	server_name,
	root,
	idx,
	error
};

conf_param resolve_conf_param(std::string param) {
	if (param == "listen") return port;
	if (param == "server_name") return server_name;
	if (param == "root") return root;
	if (param == "index") return idx;
	return error;
}

Configuration get_conf(int argc, char *argv[]) {
	Configuration conf;
	std::string file_name;

	if (argc < 2)
		file_name = std::string("default.conf");
	else
		file_name = std::string(argv[1]);
	
	std::ifstream file_stream ("conf/" + file_name);
	std::string line;

	if (!file_stream.is_open()) {// check whether the file is open
		std::cout << "Error reading conf file" << std::endl;
		exit(1);
	}

	while (file_stream) {	
		
		std::getline(file_stream, line);
		
		line = trim(line);
		if (line.find_first_of("#") != std::string::npos || line.length() == 0)
			continue;

		if (line == "server") {
			Server server;
			std::getline(file_stream, line); // go past '{'
			while (1) {
				std::getline(file_stream, line);

				line = trim(line);
				if (line.find_first_of("#") != std::string::npos || line.length() == 0)
					continue;
				if (line == "}") {
					break;
				}

				std::string param = line.substr(0, line.find_first_of(" "));
				std::string param_val = line.substr(line.find_first_of(" "), line.find_first_of(";") - line.find_first_of(" "));
				switch (resolve_conf_param(param)) {
					case port: {
						server.port = trim(param_val);
						break;
					}
					case server_name: {
						server.server_name = trim(param_val);
						break;
					}
					case root: {
						server.root = trim(param_val);
						break;
					}
					case idx: {
						server.index = trim(param_val);
						break;
					}
					case error: {
						// Break stuff
					}
				} // End switch
			} // while loop (server params)
			// conf.add_server(server); //Old conf
			conf.push_back(server);
		} // End server {}
	} // filestream while loop
	return conf;	
}

int get_listener_socket(Configuration conf, int idx) {
	struct addrinfo hints, *ai, *p;
	int listener; // socket fd
	int yes = 1;
	int rv;

	memset(&hints, 0, sizeof(hints)); // To make sure it is empty
	hints.ai_family = AF_UNSPEC; // either IPV4 or 6, no need to specify
	hints.ai_socktype = SOCK_STREAM; // TCP stream socket
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(conf[idx].server_name.c_str(), conf[idx].port.c_str(), &hints, &ai)) != 0) {
		std::cerr << "getaddrinfo error " << rv << ": " << gai_strerror(rv) << std::endl;
		exit(1);
	}

	for (p = ai; p != NULL; p = p->ai_next) {

		if ((listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
			continue;

		// If "address already in use" is a problem :
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
		
		// Makes the socket non-blocking.
		fcntl(listener, F_SETFL, O_NONBLOCK);

		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
			close(listener);
			continue;
		}

		break;
	}

	if (p == NULL)
		return -1;

	freeaddrinfo(ai); // All done with this structure

	if (listen(listener, 10) == -1) 
		return -1;
	return listener;
}

// Add new fd to the set
void add_to_pfds(struct pollfd **pfds, int new_fd, int *fd_count, int *fd_size) {
	
	if (*fd_count == *fd_size) {
		struct pollfd *tmp = new struct pollfd[*fd_size * 2];
		*fd_size *= 2;
		
		tmp = *pfds;
		delete *pfds;
		*pfds = tmp;
	}

	(*pfds)[*fd_count].fd = new_fd;
	(*pfds)[*fd_count].events = POLLIN;
	
	(*fd_count)++;
}

// Remove indexed fd from set
void del_from_pfds(struct pollfd *pfds, int idx, int *fd_count) {

	// Copy end one to the indexed fd
	pfds[idx] = pfds[*fd_count - 1];

	(*fd_count)--;
}

bool recv_header(std::string request) {
	if (request.find("\r\n\r\n") == std::string::npos)
		return true;
	return false;
}

int main(int argc, char *argv[]) {
	Configuration conf = get_conf(argc, argv);
	conf.print();

	int sockfd, new_fd;
	int fd_size = conf.size();
	struct pollfd *pfds = new struct pollfd[fd_size];
	int fd_count = 0;

	struct sockaddr_storage remoteaddr; // client's info, using sockaddr_storage because big enough to contain IPv4 or IPv6
	socklen_t addrlen; // length of remoteaddr
	char buf[10]; // Buffer for client data

	for (int i = 0; i < fd_size; i++) {
		
		sockfd = get_listener_socket(conf, i);
		if (sockfd == -1) {
			std::cerr << "listen: " << strerror(errno) << std::endl;
			exit(1);
		}

		conf[i].sockfd = sockfd;
		pfds[i].fd = sockfd;
		pfds[i].events = POLLIN;
		fd_count++;
	}

	while(1) { // main loop
		
		// last argument is timeout, in millisecs. Neg value for no timeout until response
		int poll_count = poll(pfds, fd_count, -1); 
		if (poll_count == -1) {
			std::cerr << "poll: " << strerror(errno) << std::endl;
			exit(1);
		}

		// Run through existing connections to look for data to read
		for (int i = 0; i < fd_count; i++) {
		
			// check if someone is ready to read
			if (pfds[i].revents & POLLIN) {
				
				if (i < conf.size() && pfds[i].fd == conf[i].sockfd) { // listening socket needs to be accepted

					addrlen = sizeof(remoteaddr);
					new_fd = accept(conf[i].sockfd, (struct sockaddr *)&remoteaddr, &addrlen);
					if (new_fd == -1) {
						std::cerr << "accept: " << strerror(errno) << std::endl;
					}
					else {
						add_to_pfds(&pfds, new_fd, &fd_count, &fd_size);
						std::cout << "Server: new connection from " << conf[i].server_name << " on socket " << new_fd << std::endl;
					}
				}
				else { // Socket returned by accept(), we read the data
					
					std::string request;
					while (recv_header(request)) {
						int nbytes = recv(pfds[i].fd, buf, sizeof(buf), 0);
						request.append(buf);

						if (nbytes <= 0) { // error handling
							if (nbytes == 0) {
								std::cout << "Pollserver: socket " << pfds[i].fd << " hung up" << std::endl;
							}
							else {
								std::cerr << "recv: " << strerror(errno) << std::endl;
							}

							close(pfds[i].fd);
							del_from_pfds(pfds, i, &fd_count);
						}
					}
						
					std::cout << "\n *** Msg received: *** \n" << request;
					// break;
				}
			}
		}
	}
	return 0;
}