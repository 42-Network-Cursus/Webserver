#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "std_libs.hpp"
# include "response.hpp"
// # include "utils.hpp"
# include "configuration.hpp"
# include "server.hpp"
# include "location.hpp"

# define END_HEADER "\r\n\r\n"
# define BACKLOG 10 // queue of nb of connexions waiting to be accepted by listen()


/*
	execve, dup, dup2, pipe, strerror, gai_strerror,
	errno, dup, dup2, fork, htons, htonl, ntohs, ntohl,
	select, poll, epoll (epoll_create, epoll_ctl,
	epoll_wait), kqueue (kqueue, kevent), socket,
	accept, listen, send, recv, bind, connect,
	getaddrinfo, freeaddrinfo, setsockopt, getsockname,
	getprotobyname, fcnt
*/

#endif // WEBSERV_HPP