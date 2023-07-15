#ifndef UTILS_HPP
# define UTILS_HPP

// # include <cctype>

# include "server.hpp"
# include "constantes.hpp"
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
// # include "utils.tpp" 

std::string 		&ltrim(std::string &s, const char *t = " \t\r\f\v");
std::string 		&rtrim(std::string &s, const char *t = " \t\r\f\v");
std::string 		&trim(std::string &s, const char *t = " \t\r\f\v");
bool 				skip_line(std::string line);
bool                stringToBool(std::string str);

std::string	getTextByStatusCode(int code);
std::string	intToString(int number);

bool isInVector(std::vector<std::string> vector, std::string value);

template<typename T>
bool containsValue(const std::vector<T> &vect, const T &value);

void 				configure_servers(int argc, char *argv[], std::vector<Server> *servers);
std::pair<int, int> get_idx_server_fd(std::vector<Server> &servers, int fd);

void 		get_cgi(std::string script_path, std::string &CGI, std::string &CGI_PATH);
// std::string get_body_from_cgi(std::string script);
void get_body_from_cgi(std::string script);

std::string	deleteWhiteSpace(std::string str);
std::string	getExtension(std::string str);
bool		isCGIExtension(std::string ext);
std::string getCGIbyExtension(std::string ext);
size_t 		getContentSize(std::string request);

void readBody(std::string str);

void eraseFD(int fd, std::vector<Server> servers);

// DEBUG
void	print_server_list(std::vector<Server> servers);

#endif // UTILS_HPP