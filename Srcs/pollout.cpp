#include "webserv.hpp"

void erase_fd_from_server(int fd, std::vector<Server> &servers) {
	for (size_t i = 0; i < servers.size() ; i++) {
			
			std::vector<struct pollfd> test = servers[i].getPfds();
			std::vector<struct pollfd> tmp;

			for (size_t j = 0; j < test.size() ; j++) {
				if (fd != test[j].fd)
					tmp.push_back(test[j]);
			}
			servers[i].setPfds(tmp);
	}
}

void sendResponse(int fd, Response response) {
	std::string msg = response.getResponseInString();
	size_t 		total = 0; // Bytes sent
	int 		bytesLeft = msg.length(); // Bytes left to send
	int 		bytesRead = 0;

	std::cout << "Response status code: " << response.getStatusCode() << std::endl;
	while (total < msg.length()) {
		bytesRead = send(fd, msg.c_str() + total, bytesLeft, 0);

		if (bytesRead == -1) {
			std::cout << "Error sending response\n";
			break;
		}
		if (bytesRead == 0 && bytesLeft > 0)
				continue;

		total += bytesRead;
		bytesLeft -= bytesRead;
	}
}

int get_request_index(int sockfd, std::vector<Request> requests) {

	std::vector<Request>::iterator it_begin = requests.begin();
	std::vector<Request>::iterator it_end = requests.end();
	
	int i = 0;
	int j = -1;

	for (; it_begin != it_end; it_begin++, i++) {
		if (it_begin->getSocketFd() == sockfd)
			j = i ;
	}
	return j;
}

void handle_pollout(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, int idx, std::vector<Request> &requests) {
	int sockfd = all_pfds[idx].fd;
	int req_idx = get_request_index(sockfd, requests);
	
	#ifdef DEBUG
	requests[req_idx].print();
	#endif

	Response response(requests[req_idx]);
	sendResponse(sockfd, response);

	std::vector<Request>::iterator it_begin = requests.begin();
	requests.erase(it_begin + req_idx);

	std::cout << "Pollout: Close fd " << all_pfds[idx].fd << std::endl;
	close(all_pfds[idx].fd);
	erase_fd_from_server(all_pfds[idx].fd, servers);
	all_pfds.erase(all_pfds.begin() + idx);
}