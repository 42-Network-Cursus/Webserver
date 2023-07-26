#include "webserv.hpp"

bool G_EXIT = false;

void exit_bool(int) {
	G_EXIT = true;
}

void clean_exit(std::vector<Server> &servers) {
	unsigned int i = 0;
	while (i < servers.size()) {
		servers[i].close_fds();
		i++;
	}
	exit(1);
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

	signal(SIGINT, &exit_bool);
	signal(SIGQUIT, &exit_bool);

	// main loop
	while(1) { 
		
		if (G_EXIT) {
			clean_exit(servers);
		}

		// last argument is timeout, in millisecs. Neg value for no timeout until response
		if ( (poll(all_pfds.data(), all_pfds.size(), -1)) == -1) {
			std::cerr << "\npoll: " << strerror(errno) << std::endl;
			clean_exit(servers);
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

			if (all_pfds[i].revents & POLLOUT){
				if (requests.size() == 0)
					continue ;
				
				// DELETE
				std::cout << "POLLOUT on fd " << all_pfds[i].fd << std::endl;

				handle_pollout(servers, all_pfds, i, requests);				
			}		
		}	
	}
	return 0;
}
