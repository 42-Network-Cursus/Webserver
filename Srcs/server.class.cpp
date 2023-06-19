#include "server.class.hpp"

// Debugging
void Server::print() {
	std::cout << "port: " << port << std::endl;
	std::cout << "server name: " << server_name << std::endl;
	std::cout << "root: " << root << std::endl;
	std::cout << "index: " << index << std::endl;
	std::cout << "client_max_body_size: " << client_max_body_size << std::endl;
}

// Default constructor
Server::Server() 
{
	next = NULL;
	sockfd = -1;
	client_max_body_size = "0M";
}