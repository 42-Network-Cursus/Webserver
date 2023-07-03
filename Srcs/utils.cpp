#include "webserv.hpp"

/*******************
	Debugging
*******************/ 
void		print_server_list(std::vector<Server> servers) {
	std::vector<Server>::iterator it_begin = servers.begin();
	std::vector<Server>::iterator it_end = servers.end();

	std::cout << "Number of servers: " << servers.size() << std::endl << std::endl;
	for (; it_begin != it_end; it_begin++) {
		it_begin->print();
		std::cout << std::endl;
	}
}

/*****************
 Maybe used to print addresses of connected clients ?
******************/
// get sockaddr object, IPv4 or 6
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// std::cout << "Pollserver: new connection from "
// 		<< inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN)
// 		<< " on socket " << new_fd << std::endl;


/********************
	Actual Utils
********************/

std::string& ltrim(std::string& s, const char* t) { // trim from left
    s.erase(0, s.find_first_not_of(t));
    return s;
}

std::string& rtrim(std::string& s, const char* t) { // trim from right
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

std::string& trim(std::string& s, const char* t) { // trim from left & right
    return ltrim(rtrim(s, t), t);
}

// Skips comments and empty lines in configuration file
bool skip_line(std::string line) {
	if (line.find_first_of("#") != std::string::npos || line.length() == 0)
		return true;
	return false;
}

bool stringToBool(std::string str) {
	str = trim(str);
	if (str == "true")
		return true;
	return false;
}



// void eraseFD(int fd, std::vector<Server *> servers) {
// 		for (int i = 0; servers[i] ; i++) {

// 		for (int j = 0; servers[i]->pfds[j].fd ; j++) {
// 			if (fd == servers[i]->pfds[j].fd)
// 				servers[i]->pfds.erase(servers[i]->pfds.begin() + j);
// 		}
// 	}

// }

/**
 * @brief Get the text corresponding to the status code
 * 
 * @param code Status Code
 * @return Status Code Text
 */
std::string getTextByStatusCode(int code)
{
	switch (code)
	{
		case 200:
			return SC_200;
		case 400:
			return SC_400;
		case 403:
			return SC_403;
		case 404:
			return SC_404;
		case 418:
			return SC_418;
		case 500:
			return SC_500;
		default:
			return SC_UNKNOWN;
	}
}

/**
 * @brief Convert a int to a std::string
 * 
 * @param number Number
 * @return Number value in std::string 
 */
std::string	intToString(int number)
{
	std::string res = "";
	try
	{
		std::ostringstream oss117;
		oss117 << number;
		res = oss117.str();
	}
	catch(const std::exception& e)
	{
		res = ERR_CONVERSION_ITS;
	}
	return res;
}

// Request parseRequest(std::string request)
// {
// 	std::string method = "";
// 	std::string path = "";

// 	int i = 0;
// 	while (i < 2)
// 	{
// 		if (i == 0)
// 		{
// 			//method += *request;
// 		}
// 	}

// }
