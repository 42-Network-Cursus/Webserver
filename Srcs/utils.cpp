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
		case 201:
			return SC_201;
		case 204:
			return SC_204;
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
		case 501:
			return SC_501;
		case 505:
			return SC_505;
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

int sint(std::string str)
{
	std::istringstream iss(str);
    int res;
    iss >> res;

	return (res);
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
	return (std::make_pair(-1, -1));
}

std::string deleteWhiteSpace(std::string str)
{
	std::string::const_iterator it = str.begin();
	std::string::const_iterator end = str.end();

	while (std::isspace(*it) && it != end)
		++it;

	std::string res = "";
	while (it != end)
	{
		res += *it;
		++it;
	}

	return res;
}

std::string getExtension(std::string str)
{
	size_t pos = str.find_last_of(".");
	if (pos <= 0)
		return ("");
	std::string res = str.substr(pos + 1);
	return res;
}

bool isCGIExtension(std::string ext)
{
	return (ext == "php" || ext == "py");
}

std::string getCGIbyExtension(std::string ext)
{
	if (ext == "php")
		return CGI_PHP;
	else if (ext =="py")
		return CGI_PY;
	else
		return CGI_UNKNOWN;
}

size_t getContentSize(std::string request)
{
	std::istringstream iss(request);
	std::string line;

	std::string content = "Content-Length:";
	size_t contentSize = content.size();
	
	int res = 0;
	std::string tmp;
	while (std::getline(iss, line))
	{
		std::cout << "======== Check Content-Length: " << line << std::endl;
		if (line.compare(0, contentSize, content) == 0)
		{
			tmp = line.substr(contentSize + 1);
			res = sint(tmp);
			return (res);
		}
	}
	return (res);
}

void readBody(std::string str)
{
	std::istringstream iss(str);
	std::string line;

	int i = 0;

	while (std::getline(iss, line) && i < 2)
	{
		std::cout << line << std::endl;
		i++;
	}
}

void configure_servers(int argc, char *argv[], std::vector<Server> *servers) {
	
	std::string 	file_name;
	if (argc < 2)
		file_name = std::string("default.conf");
	else
		file_name = std::string(argv[1]);
	
	std::ifstream 	file_stream (("conf/" + file_name).c_str());
	std::string 	line;

	// check whether the file is open
	if (!file_stream.is_open()) { 
		std::cout << "Error opening configuration file" << std::endl;
		exit(1);
	}

	while (file_stream) {
		
		std::getline(file_stream, line);
		
		line = trim(line);
		
		if (skip_line(line))
			continue;

		if (line == "server") {

			std::getline(file_stream, line); // go past '{'
			
			Server *server = new Server();

			server->store_server_configuration(file_stream);
			server->get_listening_socket();

			servers->push_back(*server);
		
		}

	}
}

void get_cgi(std::string script_path, std::string &CGI, std::string &CGI_PATH) {
	std::string suffix = script_path.substr(script_path.find_first_of("."));

	if (suffix == ".php") {
		std::cout << "Found suffix\n";
		CGI = PHP_CGI;
		CGI_PATH = PHP_CGI_PATH;
	}
}

std::string get_body_from_cgi(std::string script) {
 
	FILE 		*tmpFile = fopen("tmpFile.txt", "w");
	std::string body;
	std::string script_path = script;
	std::string CGI;
	std::string CGI_PATH;

	get_cgi(script_path, CGI, CGI_PATH);

    // Check if the file was opened successfully
    if (!tmpFile) {
        std::cout << "Failed to open the output file." << std::endl;
        return "";
    }

    // Command to execute the PHP script
    const char* command[] = {CGI.c_str(), "-f", script_path.c_str(), NULL};

    // Fork a child process
    pid_t pid = fork();

    if (pid == -1) {
        std::cerr << "Failed to fork a child process." << std::endl;
        return "";
    } 
    // Child process
	else if (pid == 0) {

        // Redirect the output to the file
        dup2(fileno(tmpFile), STDOUT_FILENO);

        // Execute the PHP script
        execve(CGI_PATH.c_str(), const_cast<char**>(command), NULL);
        
        // If execve returns, an error occurred
        std::cerr << "Failed to execute the PHP script." << std::endl;
        return "";
    } 
    // Parent process
	else {

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

		fclose(tmpFile);
		
		std::ifstream rFile("tmpFile.txt");
		while (!rFile.eof()) {
			std::string tmp;
			rFile >> tmp;
			body.append(tmp);
		}
			
		rFile.close();
		int result = remove("tmpFile.txt");
		std::cout << result << std::endl;

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            std::cout << "HTML output has been generated." << std::endl;
        } else {
            std::cerr << "PHP script execution failed." << std::endl;
            return "";
        }
    }

	return body;
}