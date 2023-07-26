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

// Request readRequest(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, std::pair<int, int> idx_pair, int idx)
// {
// 	std::string		header;
// 	std::string body = "";
// 	size_t	bufferSize = 2056;
// 	ssize_t 			bytesRead;
// 	unsigned char 			buffer[bufferSize];
// 	ssize_t pos;
// 	bool check;
// 	//std::cout << "\n\nIN read request" << std::endl;
// 	while (1)
// 	{
// 		// //std::cout << "On est la ? " << std::endl;
// 		// bytesRead = recv(all_pfds[idx].fd, buffer, bufferSize, 0);
// 		bytesRead = read(all_pfds[idx].fd, buffer, bufferSize);
// 		if (bytesRead <= 0)

// 		{
// 			// std::cout << "ERROR: " << strerror(errno) << std::endl;
// 			if (bytesRead == 0)
// 				std::cout << "Pollserver: socket " << all_pfds[idx].fd << " hung up" << std::endl;
// 			if (bytesRead < 0)
// 			{
// 				std::cout << "Resource temporarily unavailable" << std::endl;
// 				std::cout << strerror(errno) << std::endl;
// 				break ;
// 			}
// 			close(all_pfds[idx].fd);
// 			all_pfds.erase(all_pfds.begin() + idx);
// 			erase_fd_from_server(all_pfds[idx].fd, servers);
// 			break;
// 		}
// 		header += std::string(reinterpret_cast<char*>(buffer), bytesRead);
// 		memset(buffer, 0, bufferSize);
// 		pos = header.find("\r\n\r\n");
// 		if (pos >= 0 && pos != std::string::npos)
// 			check = false;
// 	}
// 	if (check)
// 	{
// 		// close(all_pfds[idx].fd);
// 		// all_pfds.erase(all_pfds.begin() + idx);
// 		// erase_fd_from_server(all_pfds[idx].fd, servers);

// 		return Request();
// 	}
// 	std::cout << "PREMIER Header => \n" << header << "\n\nPOS: " << pos << std::endl;
// 	Request res = Request::parseRequest(header, all_pfds[idx].fd, servers[idx_pair.first]);
// 	std::cout << "On bloque ici ?" << std::endl;
// 	if (res.getMethod() == METHOD_POST)
// 	{
// 		std::cout << "POST METHOD-> READ BODY" << std::endl;
// 		// if (pos == std::string::npos)
// 		// {
// 		// 	close(all_pfds[idx].fd);
// 		// 	all_pfds.erase(all_pfds.begin() + idx);
// 		// 	erase_fd_from_server(all_pfds[idx].fd, servers);
// 		// 	return Request();
// 		// }
// 		body = header.substr(pos + 4);
// 		bufferSize = 8000;
// 		unsigned char bodyRead[bufferSize];
// 		int size = getContentSize(header);
// 		if (size < -1)
// 		{
// 			// close(all_pfds[idx].fd);
// 			// all_pfds.erase(all_pfds.begin() + idx);
// 			// erase_fd_from_server(all_pfds[idx].fd, servers);
// 			return Request();
// 		}
// 		check = true;

// 		if (body.size() == size)
// 		{
// 			check = false;
// 			size = 0;
// 		}
// 		else
// 			size -= body.size();
		
// 		int timeout = 0;
// 		while (check)
// 		{
// 			if (size == 0)
// 			{
// 				check = false;
// 				continue;
// 			}
// 			std::cout << "On est la ?" << std::endl;
// 			// bytesRead = recv(all_pfds[idx].fd, bodyRead, bufferSize, 0);
// 			bytesRead = read(all_pfds[idx].fd, bodyRead, bufferSize);
// 			if (bytesRead == 0)
// 			{	
// 				check = false;
// 				continue ;
// 			}
// 			if (bytesRead < 0)
// 			{
// 				std::cout << "Body ByteRead: " << bytesRead << std::endl;
// 				std::cout << strerror(errno) << std::endl;
// 				if (timeout < 4)
// 				{
// 					timeout++;
// 					usleep(1);
// 					continue ;
// 				}
// 				// int ret = poll(&all_pfds[idx], 1, 1);
// 				// if (ret == -1)
// 				// {
// 					// close(all_pfds[idx].fd);
// 					// all_pfds.erase(all_pfds.begin() + idx);
// 					// erase_fd_from_server(all_pfds[idx].fd, servers);
// 					return Request();
// 				// }
// 				// continue ;
// 			}
// 			body += std::string(reinterpret_cast<char*>(bodyRead), bytesRead);
// 			memset(bodyRead, 0, bufferSize);
// 			size -= bytesRead;
// 			std::cout << "Size: " << size << std::endl;
// 			if (size < 0)
// 			{
// 				// close(all_pfds[idx].fd);
// 				// all_pfds.erase(all_pfds.begin() + idx);
// 				// erase_fd_from_server(all_pfds[idx].fd, servers);
// 				return Request();
// 			}
			
// 		}
// 		if (size != 0)
// 			return Request();
// 		res.setContentSize(getContentSize(header));
// 		res.setBody(body);
// 		std::cout << "fin de la lecture du body" << std::endl;
// 	}
// 	// close(all_pfds[idx].fd);
// 	// all_pfds.erase(all_pfds.begin() + idx);
// 	// erase_fd_from_server(all_pfds[idx].fd, servers);
// 	return res;
// }

// 	}
// }
void add_new_socket_to_pfds(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, int idx_serv, int idx) {
	int 						new_fd;
	socklen_t 					addrlen; 	// length of remoteaddr
	struct sockaddr_storage 	remoteaddr; // client's info, using
											//  sockaddr_storage because big enough to contain either IPv4 or IPv6

	#ifdef DEBUG
	//std::cout << "Connection from server " << servers[idx_serv].getServer_name() << std::endl << std::endl;
	#endif

	addrlen = sizeof(remoteaddr);
	new_fd = accept(all_pfds[idx].fd, (struct sockaddr *)&remoteaddr, &addrlen);
	
	// USEFULL ??
	int n = 1;
	setsockopt(new_fd, SOL_SOCKET, SO_REUSEADDR, &n, sizeof(n));


	fcntl(new_fd, F_SETFL, O_NONBLOCK);

	if (new_fd == -1)
		std::cerr << "accept: " << strerror(errno) << std::endl;
	else {
		struct pollfd new_pfd;

		new_pfd.fd = new_fd;
		// new_pfd.events = POLLIN;
		new_pfd.events = POLLIN | POLLOUT | POLLERR | POLLHUP;

		all_pfds.push_back(new_pfd);
		servers[idx_serv].getPfds().push_back(new_pfd);
	}
	std::cout << "New Socket on fd " << new_fd << std::endl;
}

int isContainsRequest(std::vector<Request> &requests, int fd)
{
	if (requests.size() == 0)
		return -1;

	std::vector<Request>::iterator start = requests.begin();
	std::vector<Request>::iterator end = requests.end();
	int i = 0;
	while (start != end)
	{
		if (start->getSocketFd() == fd)
			return i;
		start++;
		i++;
	}
	return -1;
}

int readRequest(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, std::pair<int, int> idx_pair, std::vector<Request> &requests, int idx)
{
	int id = isContainsRequest(requests, all_pfds[idx].fd);
	if (id == -1)
	{
		Request res(all_pfds[idx].fd);
		requests.push_back(res);
		id = isContainsRequest(requests, all_pfds[idx].fd);
	}
	ssize_t pos;
	std::cout << "STATE: " << requests[id].getState() << std::endl;
	// sleep(2);
	if (requests[id].getState() == ST_H)
	{
		size_t bufferSize = 1024;
		char buffer[bufferSize];
		ssize_t bytesRead;

		bytesRead = recv(requests[id].getSocketFd(), buffer, bufferSize, 0);
		if (bytesRead == 0)
			return (0);
		if (bytesRead < 0)
			return (-1);
		std::string data = std::string(buffer, bytesRead);
		requests[id].appendHeader(data);
		pos = data.find("\r\n\r\n");
		if (pos >= 0 && pos != std::string::npos)
			requests[id].setState(ST_B);
	}
	if (requests[id].getState() == ST_B)
	{
		std::string header = requests[id].getHeader();
		pos = header.find("POST");
		if (pos == std::string::npos)
			requests[id].setState(ST_R);
		else
		{
			std::string data;
			if (requests[id].getBodySize() == 0)
			{
				pos = header.find("\r\n\r\n");
				data = header.substr(pos + 4);
				requests[id].appendBody(data);
			}
			std::cout << header << std::endl;
			int size = getContentSize(header);
			size_t bufferSize = 8000;
			char buffer[bufferSize];
			ssize_t bytesRead;

			bytesRead = recv(requests[id].getSocketFd(), buffer, bufferSize, 0);
			if (bytesRead == 0)
				return (0);
			if (bytesRead < 0)
			{
				return (-1);
			}
			std::cout << "Bytes Read: " << bytesRead << std::endl;
			std::cout << "data:\n" << buffer << std::endl;
			data = std::string(buffer, bytesRead);
			requests[id].appendBody(data);
			if (size == requests[id].getBodySize())
			{
				requests[id].setState(ST_R);
			}
			else if (size < requests[id].getBodySize())
			{
				std::cout << "Size: " << size << std::endl;
				std::cout << "Body Size: " << requests[id].getBodySize() << std::endl;

				return (-8);
			}
			else
				return (-2);
		}
	}
	if (requests[id].getState() == ST_R)
	{
		Request ready = Request::parseRequest(requests[id].getHeader(),requests[id].getSocketFd(), servers[idx_pair.first]);
		ready.setBody(requests[id].getBody());
		ready.setContentSize(getContentSize(requests[id].getHeader()));
		requests[id] = ready;
		return (1);
	}
	return (-2);
}

int	handle_pollin(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, std::pair<int, int> idx_pair, std::vector<Request> &requests, int idx) {
	
	// check if listening socket received a connection	
	if (servers[idx_pair.first].getPfds()[idx_pair.second].fd == servers[idx_pair.first].getSockList()) {
		std::cout << "New connection on listening socket " << servers[idx_pair.first].getSockList() << std::endl;
		add_new_socket_to_pfds(servers, all_pfds, idx_pair.first, idx);
	// Not a listening socket, but ready to read. (Means a request)
	else
	{
		std::cout << "On lit la requete" << std::endl;
		int ret = readRequest(servers, all_pfds, idx_pair, requests, idx);
		std::cout << "Ret: " << ret << std::endl;
		if (ret == -2)
			return (ret);
		else if (ret <= 0)
		{
			if (ret == 0)
				std::cout << "Good Close" << std::endl;
			if (ret == -1)
				std::cout << "Not Good close A**Ho**" << std::endl;
			
			int id = isContainsRequest(requests, all_pfds[idx].fd);
			requests.erase(requests.begin() + id);

			close(all_pfds[idx].fd);
			erase_fd_from_server(all_pfds[idx].fd, servers);
			all_pfds.erase(all_pfds.begin() + idx);
			return (-1);
		}
		all_pfds[idx].events = POLLOUT;
		// Request req = readRequest(servers, all_pfds, idx_pair, idx);
		// std::cout << "On segfault ici ?" << std::endl;
		// if (req.getMethod() != REQ_INV)
		// {
		// 	// req.setSocketFd(all_pfds[idx].fd);
		// 	requests.push_back(req);
		// 	all_pfds[idx].events = POLLOUT;
		// }
		// else
		// {
		// 	std::cout << "DEBUT DU ELSE" << std::endl;
		// 	close(all_pfds[idx].fd);
		// 	erase_fd_from_server(all_pfds[idx].fd, servers);
		// 	all_pfds.erase(all_pfds.begin() + idx);
		// 	std::cout << "FIN DU ELSE" << std::endl;
		// }
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
	return (1);
}
