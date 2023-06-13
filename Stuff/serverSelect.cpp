/*
	Macros to manipulate FDs

	FD_SET(int fd, fd_set *set); -> add fd to the set
	FD_CLR(int fd, fd_set *set); -> remove fd from the set
	FD_ISSET(int fd, fd_set *set); -> return true if fd is in the set
	FD_ZERO(fd_set *set); -> clear all entries from the set
*/

#include <sys/time.h>
#include <iostream>

int main() {
	struct timeval tv; // Used for timeout
	fd_set readfds;

	tv.tv_sec = 5;
	tv.tv_usec = 500000;

	FD_ZERO(&readfds);
	FD_SET(0, &readfds);

	select(1, &readfds, NULL, NULL, &tv);

	if (FD_ISSET(0, &readfds))
		std::cout << "A key was pressed." << std::endl;
	else
		std::cout << "Timed out." << std::endl;

	return 0;

}