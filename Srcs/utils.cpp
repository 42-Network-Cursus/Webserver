#include "utils.hpp"
#include "webserv.hpp"

std::string& ltrim(std::string& s, const char* t) { // trim from left
    s.erase(0, s.find_first_not_of(t));
    return s;
}

std::string& rtrim(std::string& s, const char* t) { // trim from right
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

std::string& trim(std::string& s, const char* t) { // trim from left & right
    return ltrim(rtrim(s, t), t);
}

// Skips comments and empty lines in configuration file
bool skip_line(std::string line) {
	if (line.find_first_of("#") != std::string::npos || line.length() == 0)
		return true;
	return false;
}



/******************
 * Configuration utils
*******************/

conf_param resolve_conf_param(std::string param) {
	if (param == "listen") return port;
	if (param == "server_name") return server_name;
	if (param == "root") return root;
	if (param == "index") return idx;
	if (param == "client_max_body_size") return client_max_body_size;
	return error;
}

Configuration get_conf(int argc, char *argv[]) {
	Configuration conf;
	std::string file_name;

	if (argc < 2)
		file_name = std::string("default.conf");
	else
		file_name = std::string(argv[1]);
	
	std::ifstream file_stream ("conf/" + file_name);
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
			Server server;
			
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
						server.port = trim(param_val);
						break;
					}
					case server_name: {
						server.server_name = trim(param_val);
						break;
					}
					case root: {
						server.root = trim(param_val);
						break;
					}
					case idx: {
						server.index = trim(param_val);
						break;
					}
					case client_max_body_size: {
						server.client_max_body_size = trim(param_val);
						break;
					}
					case error: {
						// Break stuff
					}
				} // End switch
			} // while loop (server params)
			// conf.add_server(server); //Old conf
			server.sockfd = -1;
			conf.push_back(server);
		} // End server {}
	} // filestream while loop
	return conf;	
}

