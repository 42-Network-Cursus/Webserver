#include "webserv.hpp"

void configure_servers(int argc, char *argv[], std::vector<Server> *servers) {
	
	std::string 	file_name;
	if (argc < 2)
		file_name = std::string("default.conf");
	else
		file_name = std::string(argv[1]);
	
	std::ifstream 	file_stream (("conf/" + file_name).c_str());
	std::string 	line;

	// check whether the file is open
	if (!file_stream.is_open()) { 
		std::cout << "Error opening configuration file" << std::endl;
		exit(1);
	}

	while (file_stream) {
		
		std::getline(file_stream, line);
		
		line = trim(line);
		
		if (skip_line(line))
			continue;

		if (line == "server") {

			std::getline(file_stream, line); // go past '{'
			
			Server *server = new Server();

			server->store_server_configuration(file_stream);
			server->get_listening_socket();

			servers->push_back(*server);
		
		}

	}
}