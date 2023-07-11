#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "std_libs.hpp"

# include "response_header.hpp"
// # include "request.hpp"
class Request;

class Response {
	public:
		Response();
		~Response();
		Response(const Response &copy);
		Response &operator=(const Response &other);

		Response(Request request);

		void	getMethod(Request request);
		void	postMethod();
		void	putMethod();
		void	deleteMethod(Request request);

		std::string getResponseInString();

		void setBody(const std::string &body);
		
	private:
		int 			_statusCode;
		ResponseHeader	_header;
		std::string 	_body;
		std::string 	_path;

		bool	isValidPathFile();
		void	readFile();
		void	writeFile(std::string content);

		std::string getErrorPage();
};

#endif // RESPONSE_CLASS_HPP