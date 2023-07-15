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
	
	fcntl(new_fd, F_SETFL, O_NONBLOCK);

	if (new_fd == -1)
		std::cerr << "accept: " << strerror(errno) << std::endl;
	else {
		struct pollfd new_pfd;

		new_pfd.fd = new_fd;
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

		bool check = false;
		std::string tmp = "";

		// while (recv_header(request)) {
		while (check == false) {
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

			check = recv_header(buf);
			if (check) {
				size_t pos = request.find("\r\n\r\n");
				if (pos != std::string::npos)
					tmp = request.substr(pos + 4);
			}
		}

		int bodySize = getContentSize(request);
		std::string body;
		
		if (bodySize > 0) {
			char	bodyBuff[8000];
			memset(bodyBuff, '\0', 8000);
			body.resize(bodySize);

			size_t nbBytes = 0;
			while (nbBytes < bodySize) {
				//int bytes = recv(all_pfds[idx].fd, &body[nbBytes], bodySize - nbBytes, 0);
				int bytes = recv(all_pfds[idx].fd, bodyBuff, sizeof(bodyBuff), 0);
				
				if (bytes <= 0) {
					if (bytes == 0)
						std::cout << "Pollserver: socket " << all_pfds[idx].fd << " hung up" << std::endl;
					else 
						break;
					
					close(all_pfds[idx].fd);
					all_pfds.erase(all_pfds.begin() + idx);
				}
				
				nbBytes += bytes;
				body.append(bodyBuff);
				//memset(bodyBuff, '\0', 8000);
			}
			body = tmp + body;
		}

		else
			body = "";

		// std::cout << "\n\n REQUEST \n\n" << request << "\n\n\n";	
			
		Request req = Request::parseRequest(request, all_pfds[idx].fd, servers[idx_pair.first]);
		requests.push_back(req);
		req.setBody(body);
		req.setContentSize(bodySize);

		// #ifdef DEBUG
		// 	if (req.getMethod() == "POST")
		// 	{
		// 		req.print();
		// 		readBody(req.getBody());
		// 		// std::cout << req.getBody() << std::endl;
		// 		// std::cout << req.getBody() << std::endl;
		// 		// std::cout << req.getBody() << std::endl;
		// 	}
		// #endif

		all_pfds[idx].events = POLLOUT;
	}
}

int get_request_index(int sockfd, std::vector<Request> requests) {

	std::vector<Request>::iterator it_begin = requests.begin();
	std::vector<Request>::iterator it_end = requests.end();
	int i;
	int j = -1;

	for (i = 0; it_begin != it_end; it_begin++, i++) {
		if (it_begin->getSocketFd() == sockfd)
			j = i ;
	}

	return j;
}

void handle_pollout(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, int idx, std::vector<Request> &requests) {
	int sockfd = all_pfds[idx].fd;
	int req_idx = get_request_index(sockfd, requests);

	

	// Request re = requests[req_idx];
	
	#ifdef DEBUG
	requests[req_idx].print();
	#endif

	Response response(requests[req_idx]);
	sendResponse(sockfd, response);
	
	// std::vector<Request>::iterator it_erase = requests.begin();
	// requests.erase(it_erase + req_idx);
	
	close(all_pfds[idx].fd);
	all_pfds.erase(all_pfds.begin() + idx);
	eraseFD(all_pfds[idx].fd, servers);
}


void conf_error(int line_nb, std::string line) {
	
	std::cout << "Detected configuration error at line " << line_nb << ":\n";
	std::cout << line << std::endl;
	exit(1);
}

std::vector<std::string> tokenize_string(std::string line) {
	std::stringstream ss(line);
	std::string tmp;
	std::vector<std::string> tokens;

	while(getline(ss, tmp, ' ')) {
		tokens.push_back(tmp);
	}

	return tokens;
}

void check_conf_file(std::string file_name) {
	std::ifstream 	file_stream (("conf/" + file_name).c_str());
	std::string 	line;
	
	int 			line_nb = 0;
	bool 			open_server = false;
	bool 			open_location = false;

	// check whether the file is open
	if (!file_stream.is_open()) { 
		std::cout << "Error opening configuration file" << std::endl;
		exit(1);
	}

	while (file_stream) {
		
		std::getline(file_stream, line);
		line = trim(line);
		line_nb++;

		if (skip_line(line))
			continue;

		if (line == "server") {
			if (open_server){
					std::cout << "9\n";
				conf_error(line_nb, line);
			}
			
			if (line != "server"){
					std::cout << "8\n";
				conf_error(line_nb, line);
			}

			while (1) {
				std::getline(file_stream, line);
				line_nb++;
				if (skip_line(line))
					continue;
				
				if (line != "{"){
					std::cout << "7\n";
					conf_error(line_nb, line);
				}

				std::cout << "Set to true\n";
				open_server = true;
				break ;
			}
		}

		else if (line.find("location") != std::string::npos ) {
			if (!open_server || open_location) {
					std::cout << "6\n";
				conf_error(line_nb, line);
			}

			std::vector<std::string> tokens = tokenize_string(line);
			if (tokens.size() != 3 || tokens[0] != "location" || tokens[2] != "[") {
					std::cout << "5\n";
				conf_error(line_nb, line);
			}

			open_location = true;	
		}
		else { 		
			if (line == "}") {
				if (!open_server || open_location) {
					std::cout << "serv " << open_server << "loc " << open_location << std::endl;
					std::cout << "4\n";
					conf_error(line_nb, line);
				}
				else {
					std::cout << "set to false\n";
					open_server = false;
				}
			}

			else if (line == "]") {
				if (!open_server || !open_location) {
					std::cout << "3\n";
					conf_error(line_nb, line);
				}
				else
					open_location = false;
			}

			else {
				if (line == "[" || line.back() != ';') {
					std::cout << "2\n";
					conf_error(line_nb, line);
				}

				

				std::vector<std::string> tokens = tokenize_string(line);
				if (tokens.size() != 2) {
					std::cout << "1\n";
					conf_error(line_nb, line);
				}
			}
		}
		
	}

	if (open_server || open_location)
		conf_error(line_nb, line);
	
	file_stream.close();
}

int main(int argc, char *argv[]) {
	
	std::vector<Server>			servers;
	std::vector<Request>		requests;
	std::vector<struct pollfd> 	all_pfds;

	std::string 	file_name;
	if (argc > 2) {
		std::cout << "Too many args" << std::endl;
		return 1;
	}
	if (argc < 2)
		file_name = std::string("default.conf");
	else
		file_name = std::string(argv[1]);

	check_conf_file(file_name);
	configure_servers(file_name, &servers);

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
				if (idx_pair.first == -1 && idx_pair.second == -1)
					continue ;
				handle_pollin(servers, all_pfds, idx_pair, requests, i);
			}
			
			// handle POLLOUT event, socket ready to write
			if (all_pfds[i].revents & POLLOUT){
				if (requests.size() == 0)
					continue ;
				handle_pollout(servers, all_pfds, i, requests);				
			}
		}
	}
	return 0;
}
