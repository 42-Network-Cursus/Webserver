#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include <cerrno>
#include <cstring>
#include <iostream>

#include <arpa/inet.h>

#define PORT "3490"
#define MAXDATASIZE 100
#define HOST "127.0.0.1"

int sendAll(int s, char *buf, int *len) {
	int total = 0; // Bytes sent
	int bytesLeft = *len; // Bytes left to send
	int n;

	while (total < *len) {
		n = send(s, buf + total, bytesLeft, 0);
		if (n == -1)
			break;
		total += n;
		bytesLeft -= n;
	}

	*len = total; 

	return n == -1 ? -1 : 0;
}

// duplicate
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main() {
	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof(hints)); // To make sure it is empty
	hints.ai_family = AF_UNSPEC; // either IPV4 or 6, no need to specify
	hints.ai_socktype = SOCK_STREAM; // TCP stream socket
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(HOST, PORT, &hints, &servinfo)) != 0) {
		std::cerr << "getaddrinfo error: " << gai_strerror(rv) << std::endl;
		return 1;
	}

	for (p = servinfo; p != NULL; p = p->ai_next) {

		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			std::cerr << strerror(errno) << std::endl;
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			std::cerr << strerror(errno) << std::endl;
			continue;
		}

		break;
	}

	if (p == NULL) {
		std::cerr << strerror(errno) << std::endl;
		return 2;
	}

	freeaddrinfo(servinfo);


	// char msg[60] = "UNKNOWN /delete/img2.jpg HTTP/1.1\r\nHost: 127.0.0.1:3490\r\n\r\n";
	char msg[59] = "DELETE /delete/img2.jpg HTTP/1.1\r\nHost: 127.0.0.1:3490\r\n\r\n";
	// char msg[44] = "GET /upload HTTP/1.1\r\nHost: 127.0.0.1:3490\r\n\r\n";
	
	
	int *len = new int(59);
	sendAll(sockfd, msg, len);
	
	close(sockfd);

	return 0;
}