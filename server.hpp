#ifndef SERVER_HPP
# define SERVER_HPP

# include "utils.hpp"

#include <sys/types.h>
#include <sys/socket.h> // sockaddr_storage, socklen_t
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <poll.h>

#include <cerrno>
#include <cstring>
#include <iostream>

#include <arpa/inet.h> // INET6_ADDRSTRLEN macro, 

#include <fstream>

#include <vector>

#define END_HEADER "\r\n"
#define PORT "3490"
#define BACKLOG 10 // queue of nb of connexions waiting to be accepted by listen()

/*
	execve, dup, dup2, pipe, strerror, gai_strerror,
	errno, dup, dup2, fork, htons, htonl, ntohs, ntohl,
	select, poll, epoll (epoll_create, epoll_ctl,
	epoll_wait), kqueue (kqueue, kevent), socket,
	accept, listen, send, recv, bind, connect,
	getaddrinfo, freeaddrinfo, setsockopt, getsockname,
	getprotobyname, fcnt
*/


#endif // SERVER_HPP