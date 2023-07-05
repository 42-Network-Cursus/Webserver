#include "webserv.hpp"

/*******************
	Debugging
*******************/ 
void		print_server_list(std::vector<Server> servers) {
	std::vector<Server>::iterator it_begin = servers.begin();
	std::vector<Server>::iterator it_end = servers.end();

	std::cout << "Number of servers: " << servers.size() << std::endl << std::endl;
	for (; it_begin != it_end; it_begin++) {
		it_begin->print();
		std::cout << std::endl;
	}
}

/*****************
 Maybe used to print addresses of connected clients ?
******************/
// get sockaddr object, IPv4 or 6
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// std::cout << "Pollserver: new connection from "
// 		<< inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN)
// 		<< " on socket " << new_fd << std::endl;



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

void configure_servers(int argc, char *argv[], std::vector<Server *> *servers) {
	
	// std::vector<Server *> servers;
	std::string file_name;

	if (argc < 2)
		file_name = std::string("default.conf");
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
			server->_methods.push_back(METHOD_GET);
			server->_methods.push_back(METHOD_POST);
			server->_methods.push_back(METHOD_DELETE);
			
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

/********************
	Actual Utils
********************/

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

/**
 * @brief Get the text corresponding to the status code
 * 
 * @param code Status Code
 * @return Status Code Text
 */
std::string getTextByStatusCode(int code)
{
	switch (code)
	{
		case 200:
			return SC_200;
		case 400:
			return SC_400;
		case 403:
			return SC_403;
		case 404:
			return SC_404;
		case 418:
			return SC_418;
		case 500:
			return SC_500;
		default:
			return SC_UNKNOWN;
	}
}

/**
 * @brief Convert a int to a std::string
 * 
 * @param number Number
 * @return Number value in std::string 
 */
std::string	intToString(int number)
{
	std::string res = "";
	try
	{
		std::ostringstream oss117;
		oss117 << number;
		res = oss117.str();
	}
	catch(const std::exception& e)
	{
		res = ERR_CONVERSION_ITS;
	}
	return res;
}

bool isInVector(std::vector<std::string> vector, std::string value)
{
	std::vector<std::string>::iterator it = vector.begin();
	std::vector<std::string>::iterator end = vector.end();

	while(it != end)
	{
		if (*it == value)
			return true;
		++it;
	}
	std::cout << "FALSE" << std::endl;
	return false;
}