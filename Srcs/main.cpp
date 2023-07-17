#include "webserv.hpp"

// Use as signal ctrl-c ?
void clean_exit(std::vector<Server *> &servers)
{
	unsigned int i = 0;
	while (i < servers.size())
	{
		delete servers[i];
		i++;
	}
}

// first = server idx, second = pfds idx
std::pair<int, int> get_idx_server_fd(std::vector<Server> &servers, int fd)
{
	size_t i = 0;
	size_t j;
	while (i < servers.size())
	{
		j = 0;
		while (j < servers[i].getPfds().size())
		{
			if (servers[i].getPfds()[j].fd == fd)
				return (std::make_pair(i, j));
			j++;
		}
		i++;
	}
	return (std::make_pair(-1, -1));
}

int main(int argc, char *argv[]) {
	std::vector<Server>			servers;
	std::vector<Request>		requests;
	std::vector<struct pollfd> 	all_pfds;
	std::string 				file_name;

	if (argc > 2) {
		std::cout << "Too many args" << std::endl;
		return 1;
	}
	if (argc < 2)
		file_name = std::string("default.conf");
	else
		file_name = std::string(argv[1]);

	check_conf_file(file_name);
	configure_servers(file_name, &servers);

	#ifdef DEBUG
	print_server_list(servers);
	#endif

	// Fills all_pfds with listening sockets of each server
	for (size_t i = 0; i < servers.size(); i++) {
		all_pfds.push_back(servers[i].getPfds()[0]);
	}

	// signal(SIGINT, &fct_);

	// main loop
	while(1) { 
		
		// last argument is timeout, in millisecs. Neg value for no timeout until response
		if ( (poll(all_pfds.data(), all_pfds.size(), 1)) == -1) {
			std::cerr << "poll: " << strerror(errno) << std::endl;
			exit(1);
		}

		// Run through existing connections to look for data to read
		for (size_t i = 0; i < all_pfds.size(); i++) {
		
			// check if someone is ready to read
			if (all_pfds[i].revents & POLLIN) {
				std::pair<int, int> idx_pair = get_idx_server_fd(servers, all_pfds[i].fd);
				
				if (idx_pair.first == -1 && idx_pair.second == -1)
					continue ;
				handle_pollin(servers, all_pfds, idx_pair, requests, i);
			}
			
			// handle POLLOUT event, socket ready to write
			if (all_pfds[i].revents & POLLOUT){
				if (requests.size() == 0)
					continue ;
					
				handle_pollout(servers, all_pfds, i, requests);				
			}
		}
	}
	return 0;
}
