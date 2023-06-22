#include "webserv.hpp"


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
	// std::string msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-length: 112\r\n\r\n" + body;
	Response response = Response();
	response.setBody(body);

	std::string msg = response.getResponseInString();

	return sendAll(fd, msg.c_str(), msg.length());
}





int main(int argc, char *argv[]) {
	
	std::vector<Server *>			servers;
	std::vector<struct pollfd> 	all_pfds;
	int new_fd;

	configure_servers(argc, argv, &servers);
	// print_server_list(servers);

	struct sockaddr_storage remoteaddr; // client's info, using sockaddr_storage because big enough to contain IPv4 or IPv6
	socklen_t addrlen; // length of remoteaddr
	char buf[10]; // Buffer for client data

	// Fills all_pfds with listening sockets of each server
	for (size_t i = 0; i < servers.size(); i++) {
		all_pfds.push_back(servers[i]->pfds[0]);
	}

	while(1) { // main loop
		
		// last argument is timeout, in millisecs. Neg value for no timeout until response
		if ( (poll(all_pfds.data(), all_pfds.size(), 1)) == -1) {
			std::cerr << "poll: " << strerror(errno) << std::endl;
			exit(1);
		}

		// Run through existing connections to look for data to read
		for (size_t i = 0; i < all_pfds.size(); i++) {
		
			
			// check if someone is ready to read
			if (all_pfds[i].revents & POLLIN) {				
				std::cout << "POLLIN" << std::endl;
				// check if listening socket received a connection
				if (i < servers.size() && all_pfds[i].fd == servers[i]->pfds[0].fd) {

					std::cout << "Accept new connection" << std::endl;

					addrlen = sizeof(remoteaddr);
					new_fd = accept(all_pfds[i].fd, (struct sockaddr *)&remoteaddr, &addrlen);
					if (new_fd == -1) {
						std::cerr << "accept: " << strerror(errno) << std::endl;
					}
					else {
						struct pollfd new_pfd;
						new_pfd.fd = new_fd;
						new_pfd.events = POLLIN | POLLOUT;
						all_pfds.push_back(new_pfd);
						servers[i]->pfds.push_back(new_pfd);
					}
				}
				
				// Not a listening socket, but ready to read.
				else { 
					
					std::cout << "Read data" << std::endl;

					// How do i handle reading everything ?
					// 2nd while loop to read body if needed?

					std::string request;
					while (recv_header(request)) {
						int nbytes = recv(all_pfds[i].fd, buf, sizeof(buf), 0);
						request.append(buf);

						if (nbytes <= 0) { // error handling
							if (nbytes == 0) {
								std::cout << "Pollserver: socket " << all_pfds[i].fd << " hung up" << std::endl;
							}
							else {
								// We cant use errno after read
								// std::cerr << "recv: " << strerror(errno) << std::endl;
								break;
							}

							close(all_pfds[i].fd);
							all_pfds.erase(all_pfds.begin() + i);

							// Connection Management: HTTP 1.1 introduces persistent connections by default, 
							// allowing multiple requests and responses to be sent over a single TCP connection. 
							// This reduces the overhead of establishing and tearing down connections for each request, improving performance.
						}
					}
					
					std::cout << "\n\nCHECK REQUEST SAL -- OPE Tu lis jusqu'au bout ?\n" << request << "\n\n" << std::endl;
					std::cout << "\n *** Msg received on socket : " << all_pfds[i].fd << ": *** \n" << request;
					flush(std::cout);

				}
			}
			else if (all_pfds[i].revents & POLLOUT) { // handle POLLOUT event, socket ready to write
				

				std::cout << "response: " << sendResponse(all_pfds[i].fd, "<!DOCTYPE html><html><head><title>Hello, World!</title></head><body><h1>Hello, World!</h1></body></html>") << std::endl;
				std::cout << "POLLOUT\n";
				close(all_pfds[i].fd);
				all_pfds.erase(all_pfds.begin() + i);
			}
		}
	}
	return 0;
}