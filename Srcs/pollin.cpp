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
		if (line.compare(0, contentSize, content) == 0)
		{
			tmp = line.substr(contentSize + 1);
			res = sint(tmp);
			return (res);
		}
	}
	return (res);
}

Location getLocationByHostNameAndPath(std::vector<Server> &servers, std::string host, std::string path)
{
	std::vector<Server>::iterator s_begin = servers.begin();
	std::vector<Server>::iterator s_end = servers.end();

	std::vector<Location>::iterator s_loca;
	std::vector<Location>::iterator e_loca;

	host = trim(host);
	path = trim (path);
	std::string serverHost;
	std::string serverPort;
	std::string locaPath;
	while (s_begin != s_end)
	{
		serverHost = s_begin->getHost();
		serverHost = trim(serverHost);
		
		serverPort = s_begin->getPort();
		serverPort = trim(serverPort);

		serverHost = serverHost + ":" + serverPort;
		if (host.compare(0, serverHost.length(), serverHost) == 0)
		{
			s_loca = s_begin->getLocation().begin();
			e_loca = s_begin->getLocation().end();

			while (s_loca != e_loca)
			{
				locaPath = s_loca->getPath();
				if (path.compare(0, locaPath.length(), locaPath) == 0 && locaPath != "/" && path != "/")
				{
					return *s_loca;
				}
				if (path == "/")
					return *s_loca;
				++s_loca;
			}
		}
		++s_begin;
	}
	if (s_begin == s_end)
		s_begin = servers.begin();
	return *(s_begin->getLocation().begin());
}

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
	
	int n = 1;
	setsockopt(new_fd, SOL_SOCKET, SO_REUSEADDR, &n, sizeof(n));

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
	// ssize_t pos;
	size_t pos;

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

			int size = getContentSize(header);
			size_t bufferSize = 8000;
			char buffer[bufferSize];
			ssize_t bytesRead;

			if (data.size() < static_cast<size_t>(size)) {

			
				bytesRead = recv(requests[id].getSocketFd(), buffer, bufferSize, 0);
				if (bytesRead == 0)
					return (0);
				if (bytesRead < 0)
					return (-1);
			
				data = std::string(buffer, bytesRead);
				requests[id].appendBody(data);
			}
			
			if (size == requests[id].getBodySize())
				requests[id].setState(ST_R);
			else if (size < requests[id].getBodySize())
				return (-8);
			else
				return (-2);
		}
	}
	if (requests[id].getState() == ST_R)
	{
		Request ready = Request::parseRequest(requests[id].getHeader(),requests[id].getSocketFd(), servers[idx_pair.first]);
		Location loca = getLocationByHostNameAndPath(servers, ready.getHost(), ready.getPath());
		ready.setBody(requests[id].getBody());
		ready.setContentSize(getContentSize(requests[id].getHeader()));
		ready.setConfig(loca);
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
	}

	// Not a listening socket, but ready to read. (Means a request)
	else
	{
		int ret = readRequest(servers, all_pfds, idx_pair, requests, idx);
		
		if (ret == -2)
			return (ret);
		else if (ret <= 0)
		{
			int id = isContainsRequest(requests, all_pfds[idx].fd);
			requests.erase(requests.begin() + id);

			close(all_pfds[idx].fd);
			erase_fd_from_server(all_pfds[idx].fd, servers);
			all_pfds.erase(all_pfds.begin() + idx);
			return (-1);
		}
		all_pfds[idx].events = POLLOUT;
	}
	return (1);
}
