#include "webserv.hpp"

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
	size_t	bufferSize = 2056;
	ssize_t 			bytesRead;
	unsigned char 			buffer[bufferSize];
	ssize_t pos;
	bool check;
	//std::cout << "\n\nIN read request" << std::endl;
	while (1)
	{
		// //std::cout << "On est la ? " << std::endl;
		// bytesRead = recv(all_pfds[idx].fd, buffer, bufferSize, 0);
		bytesRead = read(all_pfds[idx].fd, buffer, bufferSize);
		if (bytesRead <= 0)

		{
			// std::cout << "ERROR: " << strerror(errno) << std::endl;
			if (bytesRead == 0)
				std::cout << "Pollserver: socket " << all_pfds[idx].fd << " hung up" << std::endl;
			if (bytesRead < 0)
			{
				std::cout << "Resource temporarily unavailable" << std::endl;
				std::cout << strerror(errno) << std::endl;
				break ;
			}
			close(all_pfds[idx].fd);
			all_pfds.erase(all_pfds.begin() + idx);
			erase_fd_from_server(all_pfds[idx].fd, servers);
			break;
		}
		header += std::string(reinterpret_cast<char*>(buffer), bytesRead);
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
		std::cout << "POST METHOD-> READ BODY" << std::endl;
		// if (pos == std::string::npos)
		// {
		// 	close(all_pfds[idx].fd);
		// 	all_pfds.erase(all_pfds.begin() + idx);
		// 	erase_fd_from_server(all_pfds[idx].fd, servers);
		// 	return Request();
		// }
		body = header.substr(pos + 4);
		bufferSize = 8000;
		unsigned char bodyRead[bufferSize];
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
			if (size == 0)
			{
				check = false;
				continue;
			}
			std::cout << "On est la ?" << std::endl;
			// bytesRead = recv(all_pfds[idx].fd, bodyRead, bufferSize, 0);
			bytesRead = read(all_pfds[idx].fd, bodyRead, bufferSize);
			if (bytesRead == 0)
			{	
				check = false;
				continue ;
			}
			if (bytesRead < 0)
			{
				std::cout << "Body ByteRead: " << bytesRead << std::endl;
				std::cout << strerror(errno) << std::endl;
				// int ret = poll(&all_pfds[idx], 1, 1);
				// if (ret == -1)
				// {
					close(all_pfds[idx].fd);
					all_pfds.erase(all_pfds.begin() + idx);
					erase_fd_from_server(all_pfds[idx].fd, servers);
					return Request();
				// }
				// continue ;
			}
			body += std::string(reinterpret_cast<char*>(bodyRead), bytesRead);
			memset(bodyRead, 0, bufferSize);
			size -= bytesRead;
			std::cout << "Size: " << size << std::endl;
			if (size < 0)
			{
				close(all_pfds[idx].fd);
				all_pfds.erase(all_pfds.begin() + idx);
				erase_fd_from_server(all_pfds[idx].fd, servers);
				return Request();
			}
			
		}
		if (size != 0)
			return Request();
		res.setContentSize(getContentSize(header));
		res.setBody(body);
		std::cout << "fin de la lecture du body" << std::endl;
	}
	// close(all_pfds[idx].fd);
	// all_pfds.erase(all_pfds.begin() + idx);
	// erase_fd_from_server(all_pfds[idx].fd, servers);
	return res;
}

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
		std::cout << "On segfault ici ?" << std::endl;
		if (req.getMethod() != REQ_INV)
		{
			// req.setSocketFd(all_pfds[idx].fd);
			requests.push_back(req);
			all_pfds[idx].events = POLLOUT;
		}
		// else
		// {
		// 	std::cout << "DEBUT DU ELSE" << std::endl;
		// 	close(all_pfds[idx].fd);
		// 	std::cout << "Avant le ERASE ?" << std::endl;
		// 	std::cout << "IDX: " << idx;
		// 	std::cout << " == all_pfds size: " << all_pfds.size() << std::endl;
		// 	all_pfds.erase(all_pfds.begin() + idx);
		// 	std::cout << "ICI ?" << std::endl;
		// 	erase_fd_from_server(all_pfds[idx].fd, servers);
		// 	std::cout << "FIN DU ELSE" << std::endl;
		// }
		// requests.push_back(req);
		// all_pfds[idx].events = POLLOUT;
		// }
		// else
		// 	all_pfds[idx].events = POLLIN;
	}
}
