#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <fstream>
# include <unistd.h>
# include <sys/types.h>
# include <dirent.h>

# include "response_header.hpp"
# include "request.hpp"
# include "utils.hpp"
# include "autoIndex.hpp"

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
		std::string 	_filename;

		bool	isValidPathFile();
		void	readFile();
		void	writeFile(std::string filename, std::string content);

		std::string getPageErrorStatus(int statusCode);
		bool 		checkUploadPath(std::string path);
		bool 		createDirectory(std::string path);
		std::string getErrorPage();
		std::string getDeletedFilePage();
		std::string getUploadedFilePage();
};

#endif // RESPONSE_CLASS_HPP