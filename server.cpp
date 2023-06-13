#include "server.hpp"

// int main(int argc, char **argv) {
// 	get_conf();
// 	// 	-> set up objects with info
// 	// launch_server();
// 	//  	-> open sockets
// 	// 	-> send response if needed
// 	// 	-> never close ?

// 	std::ifstream myfile ("src/index.html");
// 	std::string mystring;

// 	if ( myfile.is_open() ) { // always check whether the file is open

// 		while (myfile) {
// 			myfile >> mystring; // pipe file's content into stream
// 			std::cout << mystring; // pipe stream's content to standard output
// 		}
// 	}
// }



class Server {
	public:
	private:
		int port;
		int listening_socket;
		std::string server_name;
		std::string root;
		std::string index;
};

class Configuration {
	public:
	private:
		Server	*server_list;		
};

// trim from left
std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from right
std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}
// trim from left & right
std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
{
    return ltrim(rtrim(s, t), t);
}

Configuration get_conf(int argc, char *argv[]) {
	Configuration conf;
	
	std::string file_name;		
	if (argc < 2)
		file_name = std::string("default.conf");
	else
		file_name = std::string(argv[1]);
	
	std::ifstream file_stream ("conf/" + file_name);
	std::string line;

	if ( file_stream.is_open() ) { // always check whether the file is open
		while (file_stream) {	
			std::getline(file_stream, line);
			trim(line);
			if (line.find_first_of("#") != std::string::npos)
				continue;
			if (line.length() == 0) {
				continue;
			}
			std::cout << line << std::endl; // pipe stream's content to standard output
		}
	}
	return conf;	
}

int main(int argc, char *argv[]) {
	Configuration conf = get_conf(argc, argv);
	
	// struct pollfd *pfds;
	
	return 0;
}