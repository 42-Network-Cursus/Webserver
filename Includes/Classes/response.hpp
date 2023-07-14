#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <fstream>
# include <unistd.h>
# include <sys/types.h>
# include <dirent.h>

# include "response_header.hpp"
# include "request.hpp"

class Response {

	public:
		Response();
		~Response();
		Response(const Response &copy);
		Response &operator=(const Response &other);

		Response(Request request);

		void	getMethod(Request request);
		void	postMethod(Request request);
		void	deleteMethod(Request request);

		std::string getResponseInString();

		void setBody(const std::string &body);
		
		// DEBUG
		void print();

	private:
		int 			_statusCode;
		ResponseHeader	_header;
		std::string 	_body;
		std::string 	_path;

		bool	isValidPathFile();
		void	readFile();
		void	writeFile(std::string content);

		std::string getPageErrorStatus(int statusCode);
		bool 		checkUploadPath(std::string path);
		bool 		createDirectory(std::string path);
		std::string getErrorPage();
};

#endif // RESPONSE_CLASS_HPP