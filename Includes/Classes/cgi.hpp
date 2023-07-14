#ifndef CGI_HPP
# define CGI_HPP

#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <map>

class CGI
{
	public:
			CGI();
			~CGI();
			CGI(const CGI &copy);
			CGI &operator=(const CGI &other);

			CGI(const std::string &execPath);

			void setEnv(std::string type, std::string length, std::string method, std::string query, std::string root, std::string pathInfo, std::string redirect);
			
			std::string execute(std::string path);
	
	private:
		// Besoin de paramètres ?
		std::string _execPath;
		std::map<std::string, std::string> _env;

		std::string errorPage();


		// VARS
		// CGI PATH -->> FROM suffix script
		// SCRIPT PATH
		// ENV ?
		// tmpFile ?? ->>> Global ? Or actual tmp file ?
		//  	IF real tmp -> string content ?
};

#endif