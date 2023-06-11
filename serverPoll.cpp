#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <poll.h>

#include <cerrno>
#include <cstring>
#include <iostream>

#include <arpa/inet.h>

#define PORT "3490"
#define BACKLOG 10 // queue of nb of connexions waiting to be accepted by listen()

/*
    POLL VERSION
*/

/*
order of call:
	getaddrinfo()
	socket()
	bind()
	listen()
	accept() ->returns new socket fd, on which we can use send() and recv(), also returns -1 on error
 	send() / recv()
*/

void sigchld_handler(int s) {

	// Waitpid might overwrite errno, so we save it and restore it below
	int saved_errno = errno;

	while (waitpid(-1, NULL, WNOHANG) > 0);
	
	errno = saved_errno;
}

// get sockaddr object, IPv4 or 6
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// return a listening socket
int get_listener_socket() {
	int listener; // socket fd
	int yes = 1;
	int rv;

	struct addrinfo hints, *ai, *p;

	memset(&hints, 0, sizeof(hints)); // To make sure it is empty
	hints.ai_family = AF_UNSPEC; // either IPV4 or 6, no need to specify
	hints.ai_socktype = SOCK_STREAM; // TCP stream socket
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
		std::cerr << "getaddrinfo error: " << gai_strerror(rv) << std::endl;
		exit(1);
	}

	for (p = ai; p != NULL; p = p->ai_next) {

		if ((listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
			continue;

		// Makes the socket non-blocking.
		// Potentially not needed because of poll()
		// fcntl(sockfd, F_SETFL, O_NONBLOCK);

		// If "address already in use" is a problem :
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
			close(listener);
			continue;
		}

		break;
	}

	if (p == NULL)
		return -1;

	freeaddrinfo(ai); // All done with this structure

	if (listen(listener, 10) == -1) 
		return -1;

	return listener;
}

// Add new fd to the set
void add_to_pfds(struct pollfd *pfds[], int new_fd, int *fd_count, int *fd_size) {

	// Add more space if none left in the pfds array
	// Need to do it c++ style
	// if (*fd_count == *fd_size) {
	// 	*fd_size *= 2;

	// 	*pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size);
	// }

	(*pfds)[*fd_count].fd = new_fd;
	(*pfds)[*fd_count].events = POLLIN;
	
	(*fd_count)++;
}

// Remove indexed fd from set
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count) {

	// Copy end one to the indexed fd
	pfds[i] = pfds[*fd_count - 1];

	(*fd_count)--;
}

int main() {
	
	int listener, new_fd; // socket file descriptors
	struct sockaddr_storage remoteaddr; // client's info, using sockaddr_storage because big enough to contain IPv4 or IPv6
	socklen_t addrlen; // length of remoteaddr
	
	char buf[256]; // Buffer for client data
	char remoteIP[INET6_ADDRSTRLEN];
	// struct sigaction sa;

	int fd_count = 0;
	int fd_size = 5;
	// struct pollfd *pfds = new ...

	listener = get_listener_socket();
	if (listener == -1) {
		std::cerr << strerror(errno) << std::endl;
		exit(1);
	}

	pfds[0].fd = listener;
	pfds[0].events = POLLIN; 

	fd_count = 1;

	// sa.sa_handler = sigchld_handler;
	// sigemptyset(&sa.sa_mask);
	// sa.sa_flags = SA_RESTART;
	// if (sigaction(SIGCHLD, &sa, NULL) == -1) {
	// 	std::cerr << strerror(errno) << std::endl;
	// 	exit(1);
	// }

	// std::cout << "Server: waiting for connections..." << std::endl;

	while(1) { // main loop

		int poll_count = poll(pfds, fd_count, -1); // last argument is timeout, in millisecs. Neg value for no timeout until response
		if (poll_count == -1) {
			std::cerr << strerror(errno) << std::endl;
			exit(1)
		}

		// Run through existing connections to look for data to read
		for (int i = 0; i < fd_count; i++) {
			
			// check if someone is ready to read
			if (pfds[i].revent & POLLIN) {

				if (pfds[i].fd == listener) { // weird line ?

					addrlen = sizeof(remoteaddr);
					new_fd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
					if (new_fd == -1) {
						std::cerr << strerror(errno) << std::endl;
					}
					else { 
						add_to_pfds(&pfds, new_fd, &fd_count, &fd_size);
						std::cout << "Pollserver: new connection from "
							<< inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN)
							<< " on socket " << new_fd << std::endl;
					}
				}
			}
			else { // regular client ?
				int nbytes = recv(pfds[i].fd, buf, sizeof(buf), 0);
				int sender_fd = pfds[i].fd;

				if (nbytes <= 0) { // error handling
					if (nbytes == 0) {
						std::cout << "Pollserver: socket " << sender_fd << " hung up" << std::endl;
					}
					else {
						std::cerr << strerror(errno) << std::endl;
					}

					close(pfds[i].fd);
					del_from_pfds(pfds, i, &fd_count);
				}
				else {
					for (int j = 0; j < fd_count; j++) { // sends msg to all connected clients...
						int dest_fd = pfds[j].fd;

						if (dest_fd != listener && dest_fd != sender_fd) {
							if (send(dest_fd, buf, nbytes, 0) == -1)
								std::cerr << strerror(errno) << std::endl;
						}
					}
				}
			}
		}


		

		

		// if (!fork()) { // child process
		// 	close(sockfd); // child doesn't need listener
		// 	if (send(new_fd, "Hello World!", 12, 0) == -1) {
		// 		std::cerr << strerror(errno) << std::endl;
		// 	}
		// 	close(new_fd);
		// 	exit(0);
		// }
		// close(new_fd);
	}
	return 0;
}