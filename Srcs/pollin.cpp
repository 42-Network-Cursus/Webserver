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
		// //std::cout << "======== Check Content-Length: " << line << std::endl;
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
	std::string		header;
	std::string body = "";
	size_t	bufferSize = 32;
	ssize_t 			bytesRead;
	char 			buffer[bufferSize];
	ssize_t pos;
	bool check;
	//std::cout << "\n\nIN read request" << std::endl;
	while (1)
	{
		// //std::cout << "On est la ? " << std::endl;
		bytesRead = recv(all_pfds[idx].fd, buffer, bufferSize, 0);
		if (bytesRead <= 0)

		{
			std::cout << "ERROR: " << strerror(errno) << std::endl;
			if (bytesRead == 0)
				std::cout << "Pollserver: socket " << all_pfds[idx].fd << " hung up" << std::endl;
			if (bytesRead < 0)
			{
				std::cout << "Resource temporarily unavailable" << std::endl;
				break ;
			}
			close(all_pfds[idx].fd);
			all_pfds.erase(all_pfds.begin() + idx);
			erase_fd_from_server(all_pfds[idx].fd, servers);
			break;
		}
		header += std::string(buffer, bytesRead);
		memset(buffer, 0, bufferSize);
		pos = header.find("\r\n\r\n");
		if (pos >= 0 && pos != std::string::npos)
			check = false;
	}
	if (check)
	{
		close(all_pfds[idx].fd);
		all_pfds.erase(all_pfds.begin() + idx);
		erase_fd_from_server(all_pfds[idx].fd, servers);

		return Request();
	}
	std::cout << "PREMIER Header => \n" << header << "\n\nPOS: " << pos << std::endl;
	Request res = Request::parseRequest(header, all_pfds[idx].fd, servers[idx_pair.first]);
	std::cout << "On bloque ici ?" << std::endl;
	if (res.getMethod() == METHOD_POST)
	{
		if (pos == std::string::npos)
		{
			close(all_pfds[idx].fd);
			all_pfds.erase(all_pfds.begin() + idx);
			erase_fd_from_server(all_pfds[idx].fd, servers);
			return Request();
		}
		body = header.substr(pos + 4);
		bufferSize = 8000;
		char bodyRead[bufferSize];
		int size = getContentSize(header);
		if (size < -1)
		{
			close(all_pfds[idx].fd);
			all_pfds.erase(all_pfds.begin() + idx);
			erase_fd_from_server(all_pfds[idx].fd, servers);
			return Request();
		}
		check = true;

		if (body.size() == size)
		{
			check = false;
			size = 0;
		}
		else
			size -= body.size();
		while (check)
		{
			std::cout << "On est la ?" << std::endl;
			bytesRead = recv(all_pfds[idx].fd, bodyRead, bufferSize, 0);
			if (bytesRead == 0)
			{	
				break;
			}
			if (bytesRead < 0)
			{
				close(all_pfds[idx].fd);
				all_pfds.erase(all_pfds.begin() + idx);
				erase_fd_from_server(all_pfds[idx].fd, servers);
				return Request();
			}
			body += std::string(bodyRead, bytesRead);
			memset(bodyRead, 0, bufferSize);
			size -= bytesRead;
			if (size < 0)
			{
				close(all_pfds[idx].fd);
				all_pfds.erase(all_pfds.begin() + idx);
				erase_fd_from_server(all_pfds[idx].fd, servers);
				return Request();
			}
			if (size == 0)
				check = false;
		}
		if (size != 0)
			return Request();
		res.setContentSize(getContentSize(header));
		res.setBody(body);
	}
	// close(all_pfds[idx].fd);
	// all_pfds.erase(all_pfds.begin() + idx);
	// erase_fd_from_server(all_pfds[idx].fd, servers);
	return res;
}

// Request readRequest(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, std::pair<int, int> idx_pair, int idx)
// {
// 	std::string		header;
// 	size_t	bufferSize = 512;
// 	size_t 			bytesRead;
// 	char 			buffer[bufferSize];
// 	bool			check = true;
// 	size_t			pos;

// 	while (check)
// 	{
// 		bytesRead = recv(all_pfds[idx].fd, buffer, bufferSize, 0);
// 		if (bytesRead <= 0 || bytesRead == std::string::npos)
// 		{
// 			//std::cout << "ERROR: " << strerror(errno) << std::endl;
// 			if (bytesRead == 0 || bytesRead == std::string::npos)
// 				std::cout << "Pollserver: socket " << all_pfds[idx].fd << " hung up" << std::endl;
// 			if (bytesRead < 0)
// 			{
// 				std::cout << "Resource temporarily unavailable" << std::endl;
// 				header = "";
// 				break ;
// 			}
// 			close(all_pfds[idx].fd);
// 			all_pfds.erase(all_pfds.begin() + idx);
// 			break;
// 		}
// 		header += std::string(buffer, bufferSize);
// 		memset(buffer, 0, bufferSize);
// 		pos = header.find("\r\n\r\n");
// 		if (pos >= 0)
// 			check = false;
// 	}

// 	Request res = Request::parseRequest(header, all_pfds[idx].fd, servers[idx_pair.first]);
// 	if (res.getMethod() == METHOD_POST)
// 	{
// 		std::string body = header.substr(pos + 4);
// 		bufferSize = 8000;
// 		char bodyRead[bufferSize];
// 		int size = getContentSize(header);
// 		if (size < -1)
// 			return Request();
// 		check = true;

// 		while (check)
// 		{
// 			bytesRead = recv(all_pfds[idx].fd, bodyRead, bufferSize, 0);
// 			if (bytesRead < 0)
// 			{
// 				std::cout << "Resource temporarily unavailable" << std::endl;
					
// 				close(all_pfds[idx].fd);
// 				all_pfds.erase(all_pfds.begin() + idx);
// 				break;
// 			}

// 		}
// 	}
// }

void add_new_socket_to_pfds(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, int idx_serv, int idx) {
	int 						new_fd;
	struct sockaddr_storage 	remoteaddr; // client's info, using
	//  sockaddr_storage because big enough to contain either IPv4 or IPv6
	socklen_t 					addrlen; // length of remoteaddr

	#ifdef DEBUG
	//std::cout << "Connection from server " << servers[idx_serv].getServer_name() << std::endl << std::endl;
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
	std::cout << "New Socket on fd " << new_fd << std::endl;
}

void handle_pollin(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, std::pair<int, int> idx_pair, std::vector<Request> &requests, int idx) {
	
	// check if listening socket received a connection	
	if (servers[idx_pair.first].getPfds()[idx_pair.second].fd == servers[idx_pair.first].getSockList())
		add_new_socket_to_pfds(servers, all_pfds, idx_pair.first, idx);
	
	// Not a listening socket, but ready to read. (Means a request)
	else {
		Request req = readRequest(servers, all_pfds, idx_pair, idx);
		if (req.getMethod() != REQ_INV)
		{
			requests.push_back(req);
			all_pfds[idx].events = POLLOUT;
		}
		
	}
}
