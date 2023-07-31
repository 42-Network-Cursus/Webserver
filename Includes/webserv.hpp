#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "utils.hpp"
# include "server.hpp"
# include "request.hpp"
# include "response.hpp"

# include <cstring>
# include <stdlib.h>

# define BACKLOG 10000 // queue of nb of connexions waiting to be accepted by listen()

// COMMENT TO STOP PRINTING
// # define DEBUG


/*** SIGNAL ***/
extern bool G_EXIT;

void exit_bool(int);
void clean_exit(std::vector<Server> &servers);

/*** POLLIN && POLLOUT ***/
int 	handle_pollin(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, std::pair<int, int> idx_pair, std::vector<Request> &requests, int idx);
void	handle_pollout(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, int idx, std::vector<Request> &requests);
void	erase_fd_from_server(int fd, std::vector<Server> &servers);

bool		isFile(std::string str);
std::string getFilename(std::string body);
std::string getContentBody(std::string body);

std::string getExtensionFile(std::string str);
std::string getContentType(std::string str);

#endif // WEBSERV_HPP