#ifndef UTILS_HPP
# define UTILS_HPP

# include "server.hpp"
# include "constantes.hpp"
# include "request.hpp"

# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>

/*** DEBUG ***/
void	print_server_list(std::vector<Server> servers);

/*** UTILITY ***/
std::string 		&ltrim(std::string &s, const char *t = " \t\r\f\v");
std::string 		&rtrim(std::string &s, const char *t = " \t\r\f\v");
std::string 		&trim(std::string &s, const char *t = " \t\r\f\v");
bool 				skip_line(std::string line);
bool                stringToBool(std::string str);
std::string	        getTextByStatusCode(int code);
std::string	        intToString(int number);
int                 sint(std::string str);
std::string	        deleteWhiteSpace(std::string str);

/*** CONFIGURATION ***/
void                        conf_error(int line_nb, std::string line);
std::vector<std::string>    tokenize_string(std::string line);
void                        check_conf_file(std::string file_name);
void 			            configure_servers(std::string file_name, std::vector<Server> *servers);

/*** CGI ***/
void 						get_cgi(std::string script_path, std::string &CGI, std::string &CGI_PATH);
std::pair<std::string, int> get_body_from_cgi(Request &request);
std::string					getExtension(std::string str);
bool						isCGIExtension(std::string ext);

size_t 		getContentSize(std::string request);
bool		isFile(std::string str);

std::string getFilename(std::string body);
std::string getContentBody(std::string body);

std::string getExtensionFile(std::string str);
std::string getContentType(std::string str);

#endif // UTILS_HPP