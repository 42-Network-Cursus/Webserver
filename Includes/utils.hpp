#ifndef UTILS_HPP
# define UTILS_HPP

# include "std_libs.hpp"
# include "server.class.hpp"


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

conf_param 			resolve_conf_param(std::string param);
void configure_servers(int argc, char *argv[], std::vector<Server *> *servers);

// Debugging
void				print_server_list(std::vector<Server> servers);

#endif // UTILS_HPP