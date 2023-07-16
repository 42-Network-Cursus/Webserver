#ifndef UTILS_HPP
# define UTILS_HPP

// # include <cctype>

# include "server.hpp"
# include "constantes.hpp"
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
// # include "utils.tpp" 

// UTILITY
    // DEBUG
void	print_server_list(std::vector<Server> servers);

std::string 		&ltrim(std::string &s, const char *t = " \t\r\f\v");
std::string 		&rtrim(std::string &s, const char *t = " \t\r\f\v");
std::string 		&trim(std::string &s, const char *t = " \t\r\f\v");
bool 				skip_line(std::string line);
bool                stringToBool(std::string str);
std::string	        getTextByStatusCode(int code);
std::string	        intToString(int number);
int                 sint(std::string str);
std::string	        deleteWhiteSpace(std::string str);

// CONFIGURATION
void                        conf_error(int line_nb, std::string line);
std::vector<std::string>    tokenize_string(std::string line);
void                        check_conf_file(std::string file_name);
void 			            configure_servers(std::string file_name, std::vector<Server> *servers);

// CGI
void 		get_cgi(std::string script_path, std::string &CGI, std::string &CGI_PATH);
std::string get_body_from_cgi(std::string script);

std::string	getExtension(std::string str);
bool		isCGIExtension(std::string ext);
std::string getCGIbyExtension(std::string ext);


// template<typename T>
// bool containsValue(const std::vector<T> &vect, const T &value);


// bool isInVector(std::vector<std::string> vector, std::string value);
// void readBody(std::string str);



#endif // UTILS_HPP