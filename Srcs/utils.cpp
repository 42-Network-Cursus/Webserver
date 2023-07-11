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

std::string &ltrim(std::string &s, const char* t) { // trim from left
    s.erase(0, s.find_first_not_of(t));
    return s;
}

std::string &rtrim(std::string &s, const char* t) { // trim from right
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

std::string &trim(std::string &s, const char* t) { // trim from left & right
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



void eraseFD(int fd, std::vector<Server> servers) {
	for (size_t i = 0; i < servers.size() ; i++) {
			
			std::vector<struct pollfd> test = servers[i].getPfds();
			std::vector<struct pollfd> tmp;

			for (size_t j = 0; j < test.size() ; j++) {
				
				if (fd != test[j].fd)
					tmp.push_back(test[j]);
			}
			
			servers[i].setPfds(tmp);
	}
}

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

bool isInVector(std::vector<std::string> vector, std::string value)
{
	std::vector<std::string>::iterator it = vector.begin();
	std::vector<std::string>::iterator end = vector.end();

	while(it != end)
	{
		if (*it == value)
			return true;
		++it;
	}
	std::cout << "FALSE" << std::endl;
	return false;
}

// first = server idx, second = pfds idx
std::pair<int, int> get_idx_server_fd(std::vector<Server> &servers, int fd)
{
	size_t i = 0;
	size_t j;
	while (i < servers.size())
	{
		j = 0;
		while (j < servers[i].getPfds().size())
		{
			if (servers[i].getPfds()[j].fd == fd)
				return (std::make_pair(i, j));
			j++;
		}
		i++;
	}
	return (std::make_pair(0, 0));
}