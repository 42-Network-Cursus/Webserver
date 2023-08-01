#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string>

#include "utils.hpp"

void conf_error(int line_nb, std::string line) {
	
	std::cout << "Detected configuration error at line " << line_nb << ":\n";
	std::cout << line << std::endl;
	exit(1);
}

std::vector<std::string> tokenize_string(std::string line) {
	std::stringstream ss(line);
	std::string tmp;
	std::vector<std::string> tokens;

	while(getline(ss, tmp, ' ')) {
		tokens.push_back(tmp);
	}

	return tokens;
}

void check_conf_file(std::string file_name) {
	std::ifstream 	file_stream (("conf/" + file_name).c_str());
	std::string 	line;
	
	int 			line_nb = 0;
	bool 			open_server = false;
	bool 			open_location = false;

	// check whether the file is open
	if (!file_stream.is_open()) { 
		std::cout << "Error opening configuration file" << std::endl;
		exit(1);
	}

	while (file_stream) {
		std::getline(file_stream, line);
		line = trim(line);
		line_nb++;

		if (skip_line(line))
			continue;

		if (line == "server") {

			if (open_server)
				conf_error(line_nb, line);
			if (line != "server")
				conf_error(line_nb, line);

			while (1) {
				std::getline(file_stream, line);
				line_nb++;
				if (skip_line(line))
					continue;
				
				if (line != "{")
					conf_error(line_nb, line);

				open_server = true;
				break ;
			}
		}

		else if (line.find("location") != std::string::npos ) {
			if (!open_server || open_location)
				conf_error(line_nb, line);

			std::vector<std::string> tokens = tokenize_string(line);
			if (tokens.size() != 3 || tokens[0] != "location" || tokens[2] != "[")
				conf_error(line_nb, line);

			open_location = true;	
		}
		else { 		
			if (line == "}") {
				if (!open_server || open_location)
					conf_error(line_nb, line);
				else
					open_server = false;
			}

			else if (line == "]") {
				if (!open_server || !open_location)
					conf_error(line_nb, line);
				else
					open_location = false;
			}

			else {
				if (line == "[" || line[line.length() - 1] != ';')
					conf_error(line_nb, line);

				std::vector<std::string> tokens = tokenize_string(line);
				if (tokens.size() != 2)
					conf_error(line_nb, line);

				if (line.find("client_max_body_size") != std::string::npos) {
// 
					std::istringstream iss(line.substr(line.find(" ")));

					int strToInt;

					if (iss >> strToInt)
					{
						if (strToInt < 0)
							conf_error(line_nb, line);
					}
					else
						conf_error(line_nb, line);
				}

			}
		}
	}

	if (open_server || open_location)
		conf_error(line_nb, line);
	
	file_stream.close();
}

void configure_servers(std::string file_name, std::vector<Server> *servers) {
	
	std::ifstream 	file_stream (("conf/" + file_name).c_str());
	std::string 	line;

	// check whether the file is open
	if (!file_stream.is_open()) { 
		std::cout << "Error opening " << file_name << std::endl;
		exit(1);
	}

	while (file_stream) {
		std::getline(file_stream, line);
		if (skip_line(line))
			continue;

		line = trim(line);
		if (line == "server") {

			// go past '{'
			std::getline(file_stream, line); 
			while (skip_line(line))
				std::getline(file_stream, line);
			
			Server *server = new Server();

			server->store_server_configuration(file_stream);
			server->get_listening_socket();

			servers->push_back(*server);
		}
	}
	file_stream.close();
}
