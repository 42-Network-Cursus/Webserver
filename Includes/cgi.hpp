#ifndef CGI_HPP
# define CGI_HPP

#include <string>
#include <unistd.h>
#include <sys/type.h>

class CGI
{
	public:
			CGI();
			~CGI();
			CGI(const CGI &copy);
			CGI &operator=(const CGI &other);

			CGI(const std::string &execPath);

			void setEnv();
			
			std::string execute(std::string path);
	
	private:
		// Besoin de paramètres ?
		std::string _execPath;
		std::map<std::string, std::string> _env;

		std::string errorPage();
};

#endif