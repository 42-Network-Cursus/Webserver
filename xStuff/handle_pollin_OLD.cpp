void handle_pollin(std::vector<Server> &servers, std::vector<struct pollfd> &all_pfds, std::pair<int, int> idx_pair, std::vector<Request> &requests, int idx) {
	
	// check if listening socket received a connection	
	if (servers[idx_pair.first].getPfds()[idx_pair.second].fd == servers[idx_pair.first].getSockList())
		add_new_socket_to_pfds(servers, all_pfds, idx_pair.first, idx);
	
	// Not a listening socket, but ready to read. (Means a request)
	else 
	{
		//std::cout << "Pardon ?" << std::endl;
		// #ifdef DEBUG
		// //std::cout << "Reading request on fd " << all_pfds[idx].fd << std::endl;
		// #endif

		// std::string request;
		// request.clear();

		// char 		buf[4096]; // Buffer for client data
		// memset(buf, '\0', sizeof(buf));

		// bool check = false;
		// std::string tmp = "";

		// // while (recv_header(request)) {
		// while (check == false) {
		// 	int nbytes = recv(all_pfds[idx].fd, buf, sizeof(buf), 0);

		// 	request.append(buf);

		// 	// error handling
		// 	if (nbytes <= 0) {
		// 		if (nbytes == 0)
		// 			//std::cout << "Pollserver: socket " << all_pfds[idx].fd << " hung up" << std::endl;
		// 		else 
		// 			break;

		// 		close(all_pfds[idx].fd);
		// 		all_pfds.erase(all_pfds.begin() + idx);
		// 	}

		// 	check = recv_header(buf);
		// 	if (check) {
		// 		size_t pos = request.find("\r\n\r\n");
		// 		if (pos != std::string::npos)
		// 			tmp = request.substr(pos + 4);
		// 	}
		// }

		// int bodySize = getContentSize(request);
		// std::string body;
		
		// if (bodySize > 0) {
		// 	char	bodyBuff[8000];
		// 	memset(bodyBuff, '\0', 8000);
		// 	body.resize(bodySize);

		// 	size_t nbBytes = 0;
		// 	while (nbBytes < bodySize) {
		// 		//int bytes = recv(all_pfds[idx].fd, &body[nbBytes], bodySize - nbBytes, 0);
		// 		int bytes = recv(all_pfds[idx].fd, bodyBuff, sizeof(bodyBuff), 0);
				
		// 		if (bytes <= 0) {
		// 			if (bytes == 0)
		// 				//std::cout << "Pollserver: socket " << all_pfds[idx].fd << " hung up" << std::endl;
		// 			else 
		// 				break;
					
		// 			close(all_pfds[idx].fd);
		// 			all_pfds.erase(all_pfds.begin() + idx);
		// 		}
				
		// 		nbBytes += bytes;
		// 		body.append(bodyBuff);
		// 		//memset(bodyBuff, '\0', 8000);
		// 	}
		// 	body = tmp + body;
		// }

		// else
		// 	body = "";

		// // //std::cout << "\n\n REQUEST \n\n" << request << "\n\n\n";	
			
		// Request req = Request::parseRequest(request, all_pfds[idx].fd, servers[idx_pair.first]);
		// requests.push_back(req);
		// req.setBody(body);
		// req.setContentSize(bodySize);

		// // #ifdef DEBUG
		// // 	if (req.getMethod() == "POST")
		// // 	{
		// // 		req.print();
		// // 		readBody(req.getBody());
		// // 		// //std::cout << req.getBody() << std::endl;
		// // 		// //std::cout << req.getBody() << std::endl;
		// // 		// //std::cout << req.getBody() << std::endl;
		// // 	}
		// // #endif

		Request req = readRequest(servers, all_pfds, idx_pair, idx);
		requests.push_back(req);
		all_pfds[idx].events = POLLOUT;
	}
}
