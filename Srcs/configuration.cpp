#include "webserv.hpp"



conf_param resolve_conf_param(std::string param) {
	if (param == "listen") return port;
	if (param == "server_name") return server_name;
	if (param == "root") return root;
	if (param == "index") return idx;
	if (param == "client_max_body_size") return client_max_body_size;
	return error;
}

void configure_servers(int argc, char *argv[], std::vector<Server *> *servers) {
	
	// std::vector<Server *> servers;
	std::string file_name;

	if (argc < 2)
		file_name = std::string("same_server.conf");
	else
		file_name = std::string(argv[1]);
	
	std::ifstream file_stream (("conf/" + file_name).c_str());
	std::string line;

	if (!file_stream.is_open()) {// check whether the file is open
		std::cout << "Error reading conf file" << std::endl;
		exit(1);
	}

	while (file_stream) {
		
		std::getline(file_stream, line);
		
		line = trim(line);
		if (skip_line(line))
			continue;

		if (line == "server") {
			Server *server = new Server();
			
			std::getline(file_stream, line); // go past '{'
			while (1) {
				std::getline(file_stream, line);

				line = trim(line);
				if (skip_line(line))
					continue;
				if (line == "}")
					break;

				std::string param = line.substr(0, line.find_first_of(" "));
				std::string param_val = line.substr(line.find_first_of(" "), line.find_first_of(";") - line.find_first_of(" "));
				
				switch (resolve_conf_param(param)) {
					case port: {
						server->port = trim(param_val);
						break;
					}
					case server_name: {
						server->server_name = trim(param_val);
						break;
					}
					case root: {
						server->root = trim(param_val);
						break;
					}
					case idx: {
						server->index = trim(param_val);
						break;
					}
					case client_max_body_size: {
						server->client_max_body_size = trim(param_val);
						break;
					}
					case error: {
						// Break stuff
					}
				} // End switch 
				
			} // while loop (server params)

			server->get_listening_socket();
			servers->push_back(server);
		
		} // End server {}

	} // filestream while loop
}