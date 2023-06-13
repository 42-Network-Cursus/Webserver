#include <iostream>
#include <fstream>

int main(int argc, char **argv) {
	// read_conf();
	// 	-> set up objects with info
	// launch_server();
	//  	-> open sockets
	// 	-> send response if needed
	// 	-> never close ?

	std::ifstream myfile ("src/index.html");
	std::string mystring;

	if ( myfile.is_open() ) { // always check whether the file is open

		while (myfile) {
			myfile >> mystring; // pipe file's content into stream
			std::cout << mystring; // pipe stream's content to standard output
		}
	}
}