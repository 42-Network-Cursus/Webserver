#include "configuration.class.hpp"

Server&		Configuration::operator[] (int idx) { 
	return _server_list[idx];
}

int			Configuration::size() const {
	return _server_list.size();
}

void		Configuration::push_back(const Server& server) {
	_server_list.push_back(server);
}

std::string Configuration::getBody(int fd) {
	std::vector<Server>::iterator it_begin = _server_list.begin();
	std::vector<Server>::iterator it_end = _server_list.end();

	for (; it_begin != it_end; it_begin++) {
		if (it_begin->sockfd == fd)
			break;
	}
	
	std::string body;
	std::ifstream file_stream (it_begin->root + it_begin->index);

	if (!file_stream.is_open()) { // check whether the file is open
		std::cout << "Error reading conf file" << std::endl;
		exit(1);
	}
	
	body.assign ( 	(std::istreambuf_iterator<char>(file_stream)),
					(std::istreambuf_iterator<char>()) 
				);

	return body;
}

// Debugging
void		Configuration::print() {
	std::vector<Server>::iterator it_begin = _server_list.begin();
	std::vector<Server>::iterator it_end = _server_list.end();

	std::cout << "Number of servers: " << _server_list.size() << std::endl << std::endl;
	for (; it_begin != it_end; it_begin++) {
		it_begin->print();
		std::cout << std::endl;
	}
}