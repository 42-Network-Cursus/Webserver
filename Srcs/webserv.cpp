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

// int getServerByFd(int fd, std::vector<Server *> servers) {


// 	for (int i = 0; servers[i] ; i++) {

// 		for (int j = 0; servers[i]->pfds[j].fd ; j++) {
// 			if (fd == servers[i]->pfds[j].fd)
// 				return i;
// 		}
// 	}
// 	return -1;
// }

// ------------ FOR TESTING
// std::string fetchBody(int fd, std::vector<Server *> servers) {
// 	// std::vector<Server>::iterator it_begin = _server_list.begin();
// 	// std::vector<Server>::iterator it_end = _server_list.end();

// 	// for (; it_begin != it_end; it_begin++) {
// 	// 	if (it_begin->sockfd == fd)
// 	// 		break;
// 	// }
	
// 	int id = getServerByFd(fd, servers);

// 	std::cout << "server nb " << id << std::endl;
// 	std::string body;
	
// 	std::string htmlpage;

// 	// if (i == 0)
// 	// 	htmlpage = "index.html";
// 	// else
// 	// 	htmlpage = "secondServer.html";	
	
// 	// servers[id]->root + servers[id]->index
// 	std::ifstream file_stream ("test");

// 	if (!file_stream.is_open()) { // check whether the file is open
// 		std::cout << "Error reading conf file" << std::endl;
// 		exit(1);
// 	}
	
// 	body.assign ( 	(std::istreambuf_iterator<char>(file_stream)),
// 					(std::istreambuf_iterator<char>()) 
// 				);

// 	return body;
// }






int main(int argc, char *argv[]) {
	
	std::vector<Server>		servers;
	std::vector<struct pollfd> 	all_pfds;
	// int 						new_fd;

	configure_servers(argc, argv, &servers);
	print_server_list(servers);

	// std::cout <<  << std::endl;



	return 0;
}



// int main(int argc, char *argv[]) {
	
// 	std::vector<Server *>		servers;
// 	std::vector<struct pollfd> 	all_pfds;
// 	int 						new_fd;

// 	configure_servers(argc, argv, &servers);
// 	// print_server_list(servers);

// 	struct sockaddr_storage 	remoteaddr; // client's info, using sockaddr_storage because big enough to contain either IPv4 or IPv6
// 	socklen_t 					addrlen; // length of remoteaddr
// 	char 						buf[10]; // Buffer for client data

// 	// Fills all_pfds with listening sockets of each server
// 	for (size_t i = 0; i < servers.size(); i++) {
// 		all_pfds.push_back(servers[i]->pfds[0]);
// 	}

// 	while(1) { // main loop
		
// 		// last argument is timeout, in millisecs. Neg value for no timeout until response
// 		if ( (poll(all_pfds.data(), all_pfds.size(), 1)) == -1) {
// 			std::cerr << "poll: " << strerror(errno) << std::endl;
// 			exit(1);
// 		}

// 		// Run through existing connections to look for data to read
// 		for (size_t i = 0; i < all_pfds.size(); i++) {
		
			
// 			// check if someone is ready to read
// 			if (all_pfds[i].revents & POLLIN) {				
// 				std::cout << "POLLIN" << std::endl;
// 				// check if listening socket received a connection
// 				if (i < servers.size() && all_pfds[i].fd == servers[i]->pfds[0].fd) {

// 					std::cout << "Accept new connection" << std::endl;

// 					addrlen = sizeof(remoteaddr);
// 					new_fd = accept(all_pfds[i].fd, (struct sockaddr *)&remoteaddr, &addrlen);
// 					if (new_fd == -1) {
// 						std::cerr << "accept: " << strerror(errno) << std::endl;
// 					}
// 					else {
// 						std::cout << "New socket to communicate : " << new_fd << std::endl;
// 						struct pollfd new_pfd;
// 						new_pfd.fd = new_fd;
// 						new_pfd.events = POLLIN | POLLOUT;
// 						// new_pfd.events = POLLIN;
// 						all_pfds.push_back(new_pfd);
// 						servers[i]->pfds.push_back(new_pfd);
// 					}
// 				}
				
// 				// Not a listening socket, but ready to read.
// 				else { 
					
// 					std::cout << "Read data" << std::endl;

// 					// How do i handle reading everything ?
// 					// 2nd while loop to read body if needed?

// 					std::string request;
// 					while (recv_header(request)) {
// 						int nbytes = recv(all_pfds[i].fd, buf, sizeof(buf), 0);
// 						request.append(buf);

// 						if (nbytes <= 0) { // error handling
// 							if (nbytes == 0) {
// 								std::cout << "Pollserver: socket " << all_pfds[i].fd << " hung up" << std::endl;
// 							}
// 							else {
// 								// We cant use errno after read
// 								// std::cerr << "recv: " << strerror(errno) << std::endl;
// 								break;
// 							}

// 							close(all_pfds[i].fd);
// 							all_pfds.erase(all_pfds.begin() + i);
							

// 							// ADD TO REQUEST CLASS 
							


// 							// all_pfds[i].events = POLLOUT;
	
// 							// Connection Management: HTTP 1.1 introduces persistent connections by default, 
// 							// allowing multiple requests and responses to be sent over a single TCP connection. 
// 							// This reduces the overhead of establishing and tearing down connections for each request, improving performance.
// 						}
// 					}
					
// 					std::cout << "\n\nCHECK REQUEST SAL -- OPE Tu lis jusqu'au bout ?\n" << request << "\n\n" << std::endl;
// 					// std::cout << "\n *** Msg received on socket : " << all_pfds[i].fd << ": *** \n" << request;
// 					// flush(std::cout);
// 					std::cout << "Read request\n";

// 				}
// 			}
// 			else if (all_pfds[i].revents & POLLOUT) { // handle POLLOUT event, socket ready to write
				

// 				// std::cout << "response: " << sendResponse(all_pfds[i].fd, "<!DOCTYPE html><html><head><title>Hello, World!</title></head><body><h1>Hello, World!</h1></body></html>") << std::endl;
// 				std::cout << "response: " << sendResponse(all_pfds[i].fd, fetchBody(all_pfds[i].fd, servers)) << std::endl;


// 				 // Execute the CGI script using the Python interpreter
// 					// int result = system("python3 /cgi-bin/py-cgi.py");

// 					// if (result == -1) {
// 					// 	// Handle error executing the script
// 					// 	// ...
// 					// }
					
// 					// Process the result as needed
// 					// ...

// 				// std::cout << "response: " << sendResponse(all_pfds[i].fd, fetchBody()) << std::endl;
				
// 				std::cout << "POLLOUT, closing socket " << all_pfds[i].fd << std::endl;
				
// 				std::cout << "1" << std::endl;
// 				all_pfds.erase(all_pfds.begin() + i);
// 				std::cout << "2" << std::endl;
// 				eraseFD(all_pfds[i].fd, servers);
// 				std::cout << "3" << std::endl;
// 				close(all_pfds[i].fd);
// 			}
// 		}
// 	}
// 	return 0;
// }
