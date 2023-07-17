#include "webserv.hpp"

// bool recv_header(std::string request) {
// 	if (request.find("\r\n\r\n") == std::string::npos)
// 		return true;
// 	return false;
// }

size_t getContentSize(std::string request)
{
	std::istringstream iss(request);
	std::string line;

	std::string content = "Content-Length:";
	size_t contentSize = content.size();
	
	int res = 0;
	std::string tmp;
	while (std::getline(iss, line))
	{
		// std::cout << "======== Check Content-Length: " << line << std::endl;
		if (line.compare(0, contentSize, content) == 0)
		{
			tmp = line.substr(contentSize + 1);
			res = sint(tmp);
			return (res);
		}
	}
	return (res);
}

Request readRequest(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, std::pair<int, int> idx_pair, int idx)
{
	
	std::string		request;
	const size_t	bufferSize = 4096;
	ssize_t 		bytesRead;
	bool 			check = true;
	char 			buffer[bufferSize];
	
	memset(buffer, '\0', bufferSize);

	while (check)
	{
		// std::cout << "On est la ? " << std::endl;
		bytesRead = recv(all_pfds[idx].fd, buffer, bufferSize, 0);
		if (bytesRead <= 0) {
			if (bytesRead == 0)
				std::cout << "Pollserver: socket " << all_pfds[idx].fd << " hung up" << std::endl;
			else 
				break;

			close(all_pfds[idx].fd);
			all_pfds.erase(all_pfds.begin() + idx);
			check = false;
		}
		request += std::string(buffer, bytesRead);
	}

	std::cout << "\n\n\n\nREQUEST: " << request << std::endl;
	std::string header = "";
	std::string body = "";
	size_t 		headerEnd = request.find("\r\n\r\n");

	// std::cout << "Position de \"\r\n\r\n\": " << headerEnd << " | Fin de requete ? " << (headerEnd == std::string::npos) << std::endl;
	if (headerEnd != std::string::npos) {
		header = request.substr(0, headerEnd + 2);
		// std::cout << request.substr(headerEnd + 2) << s:;
		if (headerEnd + 4 < request.length())
			body = request.substr(headerEnd + 4);
	}
	else
		header = request;

	// std::cout << "\n\n\n====== HEADER: \n" << header << std::endl;
	// std::cout << "\n\n\n====== BODY: \n" << body << std::endl;
	
	Request res = Request::parseRequest(header, all_pfds[idx].fd, servers[idx_pair.first]);
	res.setBody(body);
	res.setContentSize(getContentSize(header));

	return res;
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
		Request req = readRequest(servers, all_pfds, idx_pair, idx);
		requests.push_back(req);
		all_pfds[idx].events = POLLOUT;
	}
}
