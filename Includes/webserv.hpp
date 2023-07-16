#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "utils.hpp"
# include "server.hpp"
# include "request.hpp"
# include "response.hpp"
# include <cstring>

# define END_HEADER "\r\n\r\n"
# define BACKLOG 10 // queue of nb of connexions waiting to be accepted by listen()

// COMMENT TO STOP PRINTING
# define DEBUG

/*
	execve, dup, dup2, pipe, strerror, gai_strerror,
	errno, dup, dup2, fork, htons, htonl, ntohs, ntohl,
	select, poll, epoll (epoll_create, epoll_ctl,
	epoll_wait), kqueue (kqueue, kevent), socket,
	accept, listen, send, recv, bind, connect,
	getaddrinfo, freeaddrinfo, setsockopt, getsockname,
	getprotobyname, fcnt
*/

// POLLIN
// bool recv_header(std::string request);
size_t	getContentSize(std::string request);
Request readRequest(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, std::pair<int, int> idx_pair, int idx);
void 	add_new_socket_to_pfds(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, int idx_serv, int idx);
void 	handle_pollin(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, std::pair<int, int> idx_pair, std::vector<Request> &requests, int idx);

// POLLOUT
void	eraseFD(int fd, std::vector<Server> servers);
int 	sendAll(int s, const char *buf, int len);
int 	sendResponse(int fd, Response response);
int 	get_request_index(int sockfd, std::vector<Request> requests);
void	handle_pollout(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, int idx, std::vector<Request> &requests);

#endif // WEBSERV_HPP