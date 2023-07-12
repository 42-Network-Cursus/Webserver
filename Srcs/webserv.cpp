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

void add_new_socket_to_pfds(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, int idx_serv, int idx) {
		
	int 						new_fd;
	struct sockaddr_storage 	remoteaddr; // client's info, using sockaddr_storage because big enough to contain either IPv4 or IPv6
	socklen_t 					addrlen; // length of remoteaddr

	#ifdef DEBUG
	std::cout << "Connection from server " << servers[idx_serv].getServer_name() << std::endl << std::endl;
	#endif

	addrlen = sizeof(remoteaddr);
	new_fd = accept(all_pfds[idx].fd, (struct sockaddr *)&remoteaddr, &addrlen);
	
	if (new_fd == -1)
		std::cerr << "accept: " << strerror(errno) << std::endl;
	else {
		struct pollfd new_pfd;

		new_pfd.fd = new_fd;
		// new_pfd.events = POLLIN | POLLOUT;
		new_pfd.events = POLLIN;

		all_pfds.push_back(new_pfd);
		servers[idx_serv].getPfds().push_back(new_pfd);
	}
}

void handle_pollin(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, std::pair<int, int> idx_pair, std::vector<Request> &requests, int idx) {
	
	// check if listening socket received a connection
	if (servers[idx_pair.first].getPfds()[idx_pair.second].fd == servers[idx_pair.first].getSockList())
		add_new_socket_to_pfds(servers, all_pfds, idx_pair.first, idx);
	
	// Not a listening socket, but ready to read. (Means a request)
	else {

		#ifdef DEBUG
		std::cout << "Reading request on fd " << all_pfds[idx].fd << std::endl;
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
		
		Request req = Request::parseRequest(request, all_pfds[idx].fd, servers);
		requests.push_back(req);
		

		#ifdef DEBUG
		req.print();
		#endif

		all_pfds[idx].events = POLLOUT;
	}
}

int get_request_index(int sockfd, std::vector<Request> requests) {

	std::vector<Request>::iterator it_begin = requests.begin();
	std::vector<Request>::iterator it_end = requests.end();
	int i;
	int j = -1;
	for (i = 0; it_begin != it_end; it_begin++, i++) {
		std::cout << "Request Nb" << i <<  std::endl;
		it_begin->print();
		std::cout << it_begin->getSocketFd() << " VS " << sockfd << std::endl;
		if (it_begin->getSocketFd() == sockfd)
		{
			std::cout << "On break !" << std::endl;
			j = i ;
		}
	}
	std::cout << "Value of i: " << j << std::endl;
	return j;
}

void handle_pollout(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, int idx, std::vector<Request> &requests) {

	// if (requests.empty())
	// 	return ;

	int sockfd = all_pfds[idx].fd;
	int req_idx = get_request_index(sockfd, requests);

	// if (req_idx == -1)
	// {
		// std::string msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-length: 108\r\n\r\n<!DOCTYPE html><html><head><title>Hello, World!</title></head><body><h1>Hello, World!</h1></body></html>\r\n\r\n";
		// sendAll(all_pfds[idx].fd, msg.c_str(), msg.length());
		// return ;
	// }
	std::cout << "On crash" << std::endl;
	std::cout << "id request: " << req_idx << std::endl;
	std::cout << "Size of requests: " << requests.size() << std::endl;
	std::cout << "requests empty ? " << requests.empty() << std::endl;

	Request re = requests[req_idx];
	re.print();
	std::cout << "Ah non..." << std::endl;

	Response response(requests[req_idx]);
	response.print();
	sendResponse(sockfd, response);
	
	// std::vector<Request>::iterator it_erase = requests.begin();
	// requests.erase(it_erase + req_idx);

	// FOR TESTING
	// std::string msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-length: 108\r\n\r\n<!DOCTYPE html><html><head><title>Hello, World!</title></head><body><h1>Hello, World!</h1></body></html>\r\n\r\n";
	// sendAll(all_pfds[idx].fd, msg.c_str(), msg.length());
	// END FOR TESTING
	
	close(all_pfds[idx].fd);
	all_pfds.erase(all_pfds.begin() + idx);
	eraseFD(all_pfds[idx].fd, servers);
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
			if (all_pfds[i].revents & POLLIN) {

				std::pair<int, int> idx_pair = get_idx_server_fd(servers, all_pfds[i].fd);
				handle_pollin(servers, all_pfds, idx_pair, requests, i);
			}
			
			// handle POLLOUT event, socket ready to write
			if (all_pfds[i].revents & POLLOUT){
				// requests[0].print();
				handle_pollout(servers, all_pfds, i, requests);				
			}
		}
	}
	return 0;
}
