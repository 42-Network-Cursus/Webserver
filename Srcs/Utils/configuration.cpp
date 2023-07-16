#include <iostream>
#include <sstream>
#include <fstream>

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
			if (open_server){
					std::cout << "9\n";
				conf_error(line_nb, line);
			}
			
			if (line != "server"){
					std::cout << "8\n";
				conf_error(line_nb, line);
			}

			while (1) {
				std::getline(file_stream, line);
				line_nb++;
				if (skip_line(line))
					continue;
				
				if (line != "{"){
					std::cout << "7\n";
					conf_error(line_nb, line);
				}

				std::cout << "Set to true\n";
				open_server = true;
				break ;
			}
		}

		else if (line.find("location") != std::string::npos ) {
			if (!open_server || open_location) {
					std::cout << "6\n";
				conf_error(line_nb, line);
			}

			std::vector<std::string> tokens = tokenize_string(line);
			if (tokens.size() != 3 || tokens[0] != "location" || tokens[2] != "[") {
					std::cout << "5\n";
				conf_error(line_nb, line);
			}

			open_location = true;	
		}
		else { 		
			if (line == "}") {
				if (!open_server || open_location) {
					std::cout << "serv " << open_server << "loc " << open_location << std::endl;
					std::cout << "4\n";
					conf_error(line_nb, line);
				}
				else {
					std::cout << "set to false\n";
					open_server = false;
				}
			}

			else if (line == "]") {
				if (!open_server || !open_location) {
					std::cout << "3\n";
					conf_error(line_nb, line);
				}
				else
					open_location = false;
			}

			else {
				if (line == "[" || line.back() != ';') {
					std::cout << "2\n";
					conf_error(line_nb, line);
				}

				

				std::vector<std::string> tokens = tokenize_string(line);
				if (tokens.size() != 2) {
					std::cout << "1\n";
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
	file_stream.close();
}
