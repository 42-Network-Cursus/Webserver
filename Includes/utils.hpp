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
bool                stringToBool(std::string str);

std::string	getTextByStatusCode(int code);
std::string	intToString(int number);

bool isInVector(std::vector<std::string> vector, std::string value);

template<typename T>
bool containsValue(const std::vector<T>& vect, const T& value);


// Debugging
void				print_server_list(std::vector<Server> servers);

# include "utils.tpp" 

void eraseFD(int fd, std::vector<Server> servers);

#endif // UTILS_HPP