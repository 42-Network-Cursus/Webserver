#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include "configuration.class.hpp"

std::string& 		ltrim(std::string& s, const char* t = " \t\r\f\v");
std::string& 		rtrim(std::string& s, const char* t = " \t\r\f\v");
std::string& 		trim(std::string& s, const char* t = " \t\r\f\v");
bool 				skip_line(std::string line);

enum conf_param {
	port,
	server_name,
	root,
	idx,
	client_max_body_size,
	error
};

// Old ver, delete later
// Configuration 		get_conf(int argc, char *argv[]);

conf_param 			resolve_conf_param(std::string param);
std::vector<Server> configure_servers(int argc, char *argv[]);

// Debugging
void				print_server_list(std::vector<Server> servers);

#endif // UTILS_HPP