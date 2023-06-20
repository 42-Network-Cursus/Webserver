#ifndef RESPONSE_CLASS_HPP
# define RESPONSE_CLASS_HPP

# include "std_libs.hpp"

class Response {
	public:

		std::string status_code;
		std::string content_type;
		std::string content_length;
		std::string body; // html code
		
	private:
};

#endif // RESPONSE_CLASS_HPP