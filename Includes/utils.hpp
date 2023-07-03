#ifndef UTILS_HPP
# define UTILS_HPP

# include "std_libs.hpp"
# include "server.hpp"

# include "constantes.hpp"
// # include "request.class.hpp"

std::string& 		ltrim(std::string& s, const char* t = " \t\r\f\v");
std::string& 		rtrim(std::string& s, const char* t = " \t\r\f\v");
std::string& 		trim(std::string& s, const char* t = " \t\r\f\v");
bool 				skip_line(std::string line);


std::string	getTextByStatusCode(int code);
std::string	intToString(int number);

// Debugging
void				print_server_list(std::vector<Server> servers);

void eraseFD(int fd, std::vector<Server *> servers);

#endif // UTILS_HPP