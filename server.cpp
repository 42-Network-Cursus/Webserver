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

		// int listening_socket;
		int port;
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
		Configuration() {
			server_list = new Server;
		}

		void add_server(Server server) {
			if (server_list->index == "") {
				*server_list = server;
			}
			else {
				Server *tmp = server_list;
				while (tmp->next != NULL) {
					tmp = tmp->next;
				}
				tmp->next = new Server(server);
			}
		};
		
		int size() {
			int rv = 0;
			Server *tmp = server_list;
			while (tmp != NULL) {
				tmp = tmp->next;
				rv++;
			}
			return rv;
		}

		// Debugging
		void print() {
			Server *tmp = server_list;
			std::cout << "Size: " << size() << std::endl << std::endl;
			for (;tmp != NULL; tmp = tmp->next) {
				tmp->print();
				std::cout << std::endl;
			}
		}

		Server	*server_list;		
	private:
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
				switch (resolve_conf_param(param)) {
					case port: {
						server.port = atoi(line.substr(line.find_first_of(" "), line.find_first_of(";")).c_str());
						break;
					}
					case server_name: {
						server.server_name = line.substr(line.find_first_of(" "), line.find_first_of(";"));
						break;
					}
					case root: {
						server.root = line.substr(line.find_first_of(" "), line.find_first_of(";"));
						break;
					}
					case idx: {
						server.index = line.substr(line.find_first_of(" "), line.find_first_of(";"));
						break;
					}
					case error: {
						// Break stuff
					}
				} // End switch
			} // while loop (server params)
			conf.add_server(server);
		} // End server {}
	} // filestream while loop
	return conf;	
}

int get_listener_socket() {
	int listener; // socket fd
	int yes = 1;
	int rv;

	struct addrinfo hints, *ai, *p;

	memset(&hints, 0, sizeof(hints)); // To make sure it is empty
	hints.ai_family = AF_UNSPEC; // either IPV4 or 6, no need to specify
	hints.ai_socktype = SOCK_STREAM; // TCP stream socket
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
		std::cerr << "getaddrinfo error: " << gai_strerror(rv) << std::endl;
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

int main(int argc, char *argv[]) {
	Configuration conf = get_conf(argc, argv);
	// -> use vector for server list ?
	conf.print();
	
	int sockfd;
	int nb_fd = conf.size();
	struct pollfd *pfds = new struct pollfd[nb_fd];

	for (int i = 0; i < nb_fd; i++) {
		
		sockfd = get_listener_socket();
		if (sockfd == -1) {
			std::cerr << "listen: " << strerror(errno) << std::endl;
			exit(1);
		}

		pfds[i].fd = sockfd;
		pfds[i].events = POLLIN; 

	}
	
	
	return 0;
}