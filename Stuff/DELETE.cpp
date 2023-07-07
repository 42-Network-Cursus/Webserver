int getServerByFd(int fd, std::vector<Server> servers) {


	for (int i = 0; i < servers.size(); i++) {

		for (int j = 0; servers[i].getPfds()[j].fd ; j++) {
			if (fd == servers[i].getPfds()[j].fd)
				return i;
		}
	}
	return -1;
}


std::string fetchBody(int fd, std::vector<Server> servers) {
	// std::vector<Server>::iterator it_begin = _server_list.begin();
	// std::vector<Server>::iterator it_end = _server_list.end();

	// for (; it_begin != it_end; it_begin++) {
	// 	if (it_begin->sockfd == fd)
	// 		break;
	// }
	
	int id = getServerByFd(fd, servers);

	std::cout << "server nb " << id << std::endl;
	std::string body;
	
	std::string htmlpage;

	// if (i == 0)
	// 	htmlpage = "index.html";
	// else
	// 	htmlpage = "secondServer.html";	
	
	// servers[id]->root + servers[id]->index
	std::ifstream file_stream ("test");

	if (!file_stream.is_open()) { // check whether the file is open
		std::cout << "Error reading conf file" << std::endl;
		exit(1);
	}
	
	body.assign ( 	(std::istreambuf_iterator<char>(file_stream)),
					(std::istreambuf_iterator<char>()) 
				);

	return body;
}