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

	Response response = Response();
	response.setBody(body);

	std::string msg = response.getResponseInString();

	return sendAll(fd, msg.c_str(), msg.length());
}





int main(int argc, char *argv[]) {
	
	std::vector<Server>			servers;
	std::vector<struct pollfd> 	all_pfds;

	configure_servers(argc, argv, &servers);
	
	// DEBUG
	print_server_list(servers);

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
				handle_pollin(servers, all_pfds, i);
			
			// handle POLLOUT event, socket ready to write
			else if (all_pfds[i].revents & POLLOUT)
				handle_pollout();				
		}
	}
	return 0;
}
