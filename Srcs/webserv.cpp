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

int sendResponse(int fd, Response response) {


	std::string msg = response.getResponseInString();

	return sendAll(fd, msg.c_str(), msg.length());
}

void add_new_socket_to_pfds(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, int idx) {
		
	int 						new_fd;
	struct sockaddr_storage 	remoteaddr; // client's info, using sockaddr_storage because big enough to contain either IPv4 or IPv6
	socklen_t 					addrlen; // length of remoteaddr

	#ifdef DEBUG
	std::cout << "Connection from server " << servers[idx].getServer_name() << std::endl << std::endl;
	#endif

	addrlen = sizeof(remoteaddr);
	new_fd = accept(all_pfds[idx].fd, (struct sockaddr *)&remoteaddr, &addrlen);
	
	if (new_fd == -1)
		std::cerr << "accept: " << strerror(errno) << std::endl;
	else {
		struct pollfd new_pfd;

		new_pfd.fd = new_fd;
		new_pfd.events = POLLIN | POLLOUT;

		all_pfds.push_back(new_pfd);
		servers[idx].getPfds().push_back(new_pfd);
		servers[idx].setSockFD(new_fd);
	}
}

void handle_pollin(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, size_t idx, std::vector<Request> &requests) {
	
	// check if listening socket received a connection
	if (idx < servers.size())
		add_new_socket_to_pfds(servers, all_pfds, idx);
	
	// Not a listening socket, but ready to read. (Means a request)
	else {

		#ifdef DEBUG
		std::cout << "Reading request "  << std::endl;
		#endif
		
		std::string request;
		request.clear();
		char 		buf[4096]; // Buffer for client data
		memset(buf, '\0', sizeof(buf));
		
		while (recv_header(request)) {

			int nbytes = recv(all_pfds[idx].fd, buf, sizeof(buf), 0);
			
			request.append(buf);

			// error handling
			if (nbytes <= 0) {

				if (nbytes == 0)
					std::cout << "Pollserver: socket " << all_pfds[idx].fd << " hung up" << std::endl;
				else 
					break;

				close(all_pfds[idx].fd);
				all_pfds.erase(all_pfds.begin() + idx);
			}
		}
		
		#ifdef DEBUG
		std::cout << request << "\n\n" << std::endl;
		#endif
		
		Request req;
		requests.push_back(Request::parseRequest(request, all_pfds[idx].fd, servers));
		
		#ifdef DEBUG
		req.print();
		#endif
	}
}

int get_request_index(int sockfd, std::vector<Request> requests) {

	std::vector<Request>::iterator it_begin = requests.begin();
	std::vector<Request>::iterator it_end = requests.end();
	int i;
	
	for (i = 0; it_begin != it_end; it_begin++, i++) {
		if (it_begin->getSocketFd() == sockfd) {
			
			// #ifdef DEBUG
			std::cout << "Request socket found" << std::endl;
			// #endif
			
			break ;
		}
	}

	#ifdef DEBUG
	// std::cout << "request index: " << i << std::endl;
	#endif

	return i;
}

void handle_pollout(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, int idx, std::vector<Request> &requests) {

	int sockfd = all_pfds[idx].fd;
	int req_idx = get_request_index(sockfd, requests);

	std::cout << "lIGNE AVANT LE CRASH" << req_idx << std::endl;
	
	
	std::cout << " Empty ? " << requests.empty() << std::endl;
	// requests[req_idx].print();

	// Response response(requests[req_idx]);
	
	// sendResponse(sockfd, response);
	




	// response = Response(requests[i]);

	
	// requests[i].generateResponse();
	// requests[i].sendResponse(sockfd);


	// std::vector<Request>::iterator it_erase = requests.begin();
	
	// requests.erase(it_erase + req_idx);

	// std::string msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-length: 112\r\n\r\n<!DOCTYPE html><html><head><title>Hello, World!</title></head><body><h1>Hello, World!</h1></body></html>\r\n\r\n";

	// sendAll(all_pfds[idx].fd, msg.c_str(), msg.length());

	close(all_pfds[idx].fd);

	std::cout << "Apres Envoi" << std::endl;
	all_pfds.erase(all_pfds.begin() + idx);
	std::cout << "Apres .erase" << std::endl;
	eraseFD(all_pfds[idx].fd, servers);
	std::cout << "SEGFAULT" << std::endl;
	
	
}

int main(int argc, char *argv[]) {
	
	std::vector<Server>			servers;
	std::vector<Request>		requests;
	std::vector<struct pollfd> 	all_pfds;

	configure_servers(argc, argv, &servers);
	
	#ifdef DEBUG
	print_server_list(servers);
	#endif

	// Fills all_pfds with listening sockets of each server
	for (size_t i = 0; i < servers.size(); i++) {
		all_pfds.push_back(servers[i].getPfds()[0]);
	}

	// main loop
	while(1) { 
		
		// last argument is timeout, in millisecs. Neg value for no timeout until response
		if ( (poll(all_pfds.data(), all_pfds.size(), 1)) == -1) {
			std::cerr << "poll: " << strerror(errno) << std::endl;
			exit(1);
		}

		// Run through existing connections to look for data to read
		for (size_t i = 0; i < all_pfds.size(); i++) {
		
			// check if someone is ready to read
			if (all_pfds[i].revents & POLLIN)
				handle_pollin(servers, all_pfds, i, requests);
			
			// handle POLLOUT event, socket ready to write
			else if (all_pfds[i].revents & POLLOUT){
				// requests[0].print();
				handle_pollout(servers, all_pfds, i, requests);				
			}
		}
	}
	return 0;
}
