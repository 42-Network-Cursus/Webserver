#ifndef RESPONSE_CLASS_HPP
# define RESPONSE_CLASS_HPP

# include "std_libs.hpp"

# include "responseHeader.class.hpp"

class Response {
	public:
		Response();
		~Response();
		Response(const Response &copy);
		Response &operator=(const Response &other);

		void	getMethod();
		void	postMethod();
		void	putMethod();
		void	deleteMethod();

		std::string getResponseInString();

		void setBody(const std::string &body);

	private:
		int _statusCode;
		ResponseHeader _header;
		std::string _body;
		std::string _path;
};

#endif // RESPONSE_CLASS_HPP