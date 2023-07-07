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

int getServerByFd(int fd, std::vector<Server> servers) {


	for (int i = 0; i < servers.size(); i++) {

		for (int j = 0; servers[i].getPfds()[j].fd ; j++) {
			if (fd == servers[i].getPfds()[j].fd)
				return i;
		}
	}
	return -1;
}


std::string fetchBody(int fd, std::vector<Server> servers) {
	// std::vector<Server>::iterator it_begin = _server_list.begin();
	// std::vector<Server>::iterator it_end = _server_list.end();

	// for (; it_begin != it_end; it_begin++) {
	// 	if (it_begin->sockfd == fd)
	// 		break;
	// }
	
	int id = getServerByFd(fd, servers);

	std::cout << "server nb " << id << std::endl;
	std::string body;
	
	std::string htmlpage;

	// if (i == 0)
	// 	htmlpage = "index.html";
	// else
	// 	htmlpage = "secondServer.html";	
	
	// servers[id]->root + servers[id]->index
	std::ifstream file_stream ("test");

	if (!file_stream.is_open()) { // check whether the file is open
		std::cout << "Error reading conf file" << std::endl;
		exit(1);
	}
	
	body.assign ( 	(std::istreambuf_iterator<char>(file_stream)),
					(std::istreambuf_iterator<char>()) 
				);

	return body;
}


void add_new_socket_to_pfds(std::vector<Server>& servers, std::vector<struct pollfd>& all_pfds, int idx) {
		
	int 						new_fd;
	struct sockaddr_storage 	remoteaddr; // client's info, using sockaddr_storage because big enough to contain either IPv4 or IPv6
	socklen_t 					addrlen; // length of remoteaddr

	// DEBUG
	std::cout << "Connection from server " << servers[idx].getServer_name() << std::endl << std::endl;

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
	}
}

void handle_pollin(std::vector<Server>& servers, std::vector<struct pollfd>& all_pfds, int idx) {
	
	

	// check if listening socket received a connection
	if (idx < servers.size())
		add_new_socket_to_pfds(servers, all_pfds, idx);
	
	// Not a listening socket, but ready to read. (Means a request)
	else {

		std::string request;
		char 		buf[1]; // Buffer for client data
		
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
		
		// DEBUG
		std::cout << request << "\n\n" << std::endl;
		
		// Create request object
	}
}

void handle_pollout() {
	// std::cout << "response: " << sendResponse(all_pfds[i].fd, fetchBody(all_pfds[i].fd, servers)) << std::endl;


				 // Execute the CGI script using the Python interpreter
					// int result = system("python3 /cgi-bin/py-cgi.py");

					// if (result == -1) {
					// 	// Handle error executing the script
					// 	// ...
					// }
					
					// Process the result as needed
					// ...
		
			// SEGFAULT
				// all_pfds.erase(all_pfds.begin() + i);
				// eraseFD(all_pfds[i].fd, servers);
				// close(all_pfds[i].fd);
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
