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
	// std::cout << "On readRequest" << std::endl;

	std::string request;
	const size_t bufferSize = 4096;
	char buffer[bufferSize];
	memset(buffer, '\0', bufferSize);


	ssize_t bytesRead;
	bool check = true;
	while (check)
	{
		std::cout << "On est la ? " << std::endl;
		bytesRead = recv(all_pfds[idx].fd, buffer, bufferSize, 0);
		if (bytesRead <= 0){
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
	// std::cout << "REQUEST: " << request << std::endl;
	std::string header = "";
	std::string body = "";

	size_t headerEnd = request.find("\r\n\r\n");
	// std::cout << "Position de \"\r\n\r\n\": " << headerEnd << " | Fin de requete ? " << (headerEnd == std::string::npos) << std::endl;
	if (headerEnd != std::string::npos)
	{
		header = request.substr(0, headerEnd + 2);
		// std::cout << request.substr(headerEnd + 2) << s:;
		if (headerEnd + 4 < request.length())
			body = request.substr(headerEnd + 4);
	}
	else
		header = request;
	std::cout << "\n\n\n====== HEADER: \n" << header << std::endl;
	std::cout << "\n\n\n====== BODY: \n" << body << std::endl;
	
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
	else 
	{
		std::cout << "Pardon ?" << std::endl;
		// #ifdef DEBUG
		// std::cout << "Reading request on fd " << all_pfds[idx].fd << std::endl;
		// #endif

		// std::string request;
		// request.clear();

		// char 		buf[4096]; // Buffer for client data
		// memset(buf, '\0', sizeof(buf));

		// bool check = false;
		// std::string tmp = "";

		// // while (recv_header(request)) {
		// while (check == false) {
		// 	int nbytes = recv(all_pfds[idx].fd, buf, sizeof(buf), 0);

		// 	request.append(buf);

		// 	// error handling
		// 	if (nbytes <= 0) {
		// 		if (nbytes == 0)
		// 			std::cout << "Pollserver: socket " << all_pfds[idx].fd << " hung up" << std::endl;
		// 		else 
		// 			break;

		// 		close(all_pfds[idx].fd);
		// 		all_pfds.erase(all_pfds.begin() + idx);
		// 	}

		// 	check = recv_header(buf);
		// 	if (check) {
		// 		size_t pos = request.find("\r\n\r\n");
		// 		if (pos != std::string::npos)
		// 			tmp = request.substr(pos + 4);
		// 	}
		// }

		// int bodySize = getContentSize(request);
		// std::string body;
		
		// if (bodySize > 0) {
		// 	char	bodyBuff[8000];
		// 	memset(bodyBuff, '\0', 8000);
		// 	body.resize(bodySize);

		// 	size_t nbBytes = 0;
		// 	while (nbBytes < bodySize) {
		// 		//int bytes = recv(all_pfds[idx].fd, &body[nbBytes], bodySize - nbBytes, 0);
		// 		int bytes = recv(all_pfds[idx].fd, bodyBuff, sizeof(bodyBuff), 0);
				
		// 		if (bytes <= 0) {
		// 			if (bytes == 0)
		// 				std::cout << "Pollserver: socket " << all_pfds[idx].fd << " hung up" << std::endl;
		// 			else 
		// 				break;
					
		// 			close(all_pfds[idx].fd);
		// 			all_pfds.erase(all_pfds.begin() + idx);
		// 		}
				
		// 		nbBytes += bytes;
		// 		body.append(bodyBuff);
		// 		//memset(bodyBuff, '\0', 8000);
		// 	}
		// 	body = tmp + body;
		// }

		// else
		// 	body = "";

		// // std::cout << "\n\n REQUEST \n\n" << request << "\n\n\n";	
			
		// Request req = Request::parseRequest(request, all_pfds[idx].fd, servers[idx_pair.first]);
		// requests.push_back(req);
		// req.setBody(body);
		// req.setContentSize(bodySize);

		// // #ifdef DEBUG
		// // 	if (req.getMethod() == "POST")
		// // 	{
		// // 		req.print();
		// // 		readBody(req.getBody());
		// // 		// std::cout << req.getBody() << std::endl;
		// // 		// std::cout << req.getBody() << std::endl;
		// // 		// std::cout << req.getBody() << std::endl;
		// // 	}
		// // #endif
		Request req = readRequest(servers, all_pfds, idx_pair, idx);
		requests.push_back(req);
		all_pfds[idx].events = POLLOUT;
	}
}
