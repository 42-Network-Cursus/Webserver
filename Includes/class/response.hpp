#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>

class Response {
	public:

		std::string status_code;
		std::string content_type;
		std::string content_length;
		std::string body; // html code
	private:
};

#endif