#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include "std_libs.hpp"

// enum conf_param {
// 	port,
// 	server_name,
// 	root,
// 	idx,
// 	client_max_body_size,
// 	error
// };

// conf_param 			resolve_conf_param(std::string param);
void 				configure_servers(int argc, char *argv[], std::vector<Server> *servers);

#endif // CONFIGURATION_HPP


/*
	To do

		Location class -> vector
		redo parsing of conf file

		think of server_name and how to use in rest of code

		Wtf happens if same host:port for 2 servers ?
			-> ask chat

		conf checker -> if no useable server, break shit
		

*/ 