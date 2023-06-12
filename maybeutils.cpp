/*
	int getaddrinfo(
		const char *node, -> host name / IP address to connect to (ex: www.ex.com / IP) IF NULL, fills hints with our IP
		const char *service, -> port number
		const struct addrinfo *hints, -> prob no need, send an empty one
		struct addrinfo **res -> fills your given pointer with result
	)

	struct addrinfo {
               int              ai_flags;
               int              ai_family;
               int              ai_socktype;
               int              ai_protocol;
               socklen_t        ai_addrlen;
               struct sockaddr *ai_addr;
               char            *ai_canonname;
               struct addrinfo *ai_next;
           };
*/

// Print IP address
	// struct addrinfo *p;
	// for (p = res; p != NULL; p->ai_next) { // ! infinite loop
	// 	void *addr;
	// 	std::string ipver[2] = {"IPv4", "IPv6"};
	// 	int ver;


	// 	if (p->ai_family == AF_INET) { //IPV4
	// 		struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
	// 		addr = &(ipv4->sin_addr);
	// 		ver = 0;
	// 	}
	// 	else { // IPv6
	// 		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
	// 		addr = &(ipv6->sin6_addr);
	// 		ver = 1;
	// 	}

	// 	char ipstr[INET6_ADDRSTRLEN];
	// 	inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
	// 	std::cout << ipver[ver] << ":" << ipstr << std::endl;
	// }


	// Does not need bind before, kernel will choose local port for us
	// connect(sockfd, res->ai_addr, res->ai_addrlen); // -1 on error


	// send()
	//  -> returns nb of bytes sent out. Can be less than nb told to send.
	//  -> -1 on error

	// int recv(int sockfd, void *buf, int len, int flags)
	//  -> reads from sockfd into buf, up to len size
	//  -> returns nb of bytes read
	//  -> -1 on error
	//  -> 0 returned means CONNEXION CLOSED

	// sendto() / recvfrom()

	// Use close() or shutdown() to close connexion
	// int shutdown(int sockfd, int how) returns 0 on succes, -1 on error
	//  int how:
	// 		0 Further receives are disallowed
	// 		1 Further sends are disallowed
	//      2 0 + 1 (so == close() )
	//  ! shutdown only changes usability, does not free a socket, so you still need to use close()
	close(sockfd);
	shutdown(sockfd, 2);

	// gethostname() to get IP adress of computer being used. Not allowed in project but maybe needed for testing
