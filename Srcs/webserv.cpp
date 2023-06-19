#include "webserv.hpp"

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

int sendAll(int s, const char *buf, int len) {
	int total = 0; // Bytes sent
	int bytesLeft = len; // Bytes left to send
	int n;

	while (total < len) {
		n = send(s, buf + total, bytesLeft, 0);
		if (n == -1)
			break;
		total += n;
		bytesLeft -= n;
	}

	len = total;

	return n == -1 ? -1 : 0;
}

int sendResponse(int fd, std::string body) {

	// <!DOCTYPE html><html><head><title>Hello, World!</title></head><body><h1>Hello, World!</h1></body></html>
	// std::string msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-length: 112\r\n\r\n<!DOCTYPE html><html><head><title>Hello, World!</title></head><body><h1>Hello, World!</h1></body></html>\r\n\r\n";
	std::string msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + body;


	return sendAll(fd, msg.c_str(), msg.length());
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

		std::cout << "Listener socket nb: " << sockfd << std::endl;
		conf[i].list_sock = sockfd;
		pfds[i].fd = sockfd;
		pfds[i].events = POLLIN;
		fd_count++;
	}

	while(1) { // main loop
		
		// last argument is timeout, in millisecs. Neg value for no timeout until response
		int poll_count = poll(pfds, fd_count, 1);
		if (poll_count == -1) {
			std::cerr << "poll: " << strerror(errno) << std::endl;
			exit(1);
		}

		// Run through existing connections to look for data to read
		for (int i = 0; i < fd_count; i++) {
		
			// check if someone is ready to read
			if (pfds[i].revents & POLLIN) {
				
				if (i < conf.size() && pfds[i].fd == conf[i].list_sock) { // listening socket needs to be accepted

					std::cout << "\n\n ACCEPT \n\n";
					std::cout << "fd: " << conf[i].sockfd << std::endl;
					if (conf[i].sockfd == -1) {
					addrlen = sizeof(remoteaddr);
					new_fd = accept(conf[i].list_sock, (struct sockaddr *)&remoteaddr, &addrlen);
					if (new_fd == -1) {
						std::cerr << "accept: " << strerror(errno) << std::endl;
					}
					else {
						conf[i].sockfd = new_fd;
						add_to_pfds(&pfds, new_fd, &fd_count, &fd_size);
						std::cout << "Server: new connection from " << conf[i].server_name << " on socket " << new_fd << std::endl;
					}
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
								break;
							}

							close(pfds[i].fd);
							del_from_pfds(pfds, i, &fd_count);

							// Connection Management: HTTP 1.1 introduces persistent connections by default, 
							// allowing multiple requests and responses to be sent over a single TCP connection. 
							// This reduces the overhead of establishing and tearing down connections for each request, improving performance.
						}
					}
						
					std::cout << "\n *** Msg received on socket : " << pfds[i].fd << ": *** \n" << request;
					flush(std::cout);

					std::cout << "response: " << sendResponse(pfds[i].fd, conf.getBody(pfds[i].fd)) << std::endl;
					
					// std::cout << "Body: " << conf.getBody(pfds[i].fd) << std::endl;

					// request.clear();
					// memset(&buf, 0, sizeof(buf));
					// break;
				}
			}
		}
	}
	return 0;
}