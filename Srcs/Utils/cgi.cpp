#include <iostream>
#include <fstream>
#include <signal.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <sstream>
#include <iomanip>


#include "utils.hpp"

void get_cgi(std::string script_path, std::string &CGI, std::string &CGI_PATH) {
	// std::string suffix = script_path.substr(script_path.find_first_of("."));
	std::string suffix = script_path.substr(script_path.find_last_of("."));

	if (suffix == ".php") {
		// //std::cout << "Found suffix\n";
		CGI = PHP_CGI;
		CGI_PATH = PHP_CGI_PATH;
	}
}

	std::string get_body_from_cgi(std::string script, std::string postData) {
	
		FILE 		*tmpFile = fopen("tmpFile.txt", "w");
		std::string body = "";
		std::string script_path = trim(script);
		std::string CGI;
		std::string CGI_PATH;
		int pipes[2];

		std::cout << "SCRIPT PATH : " << script_path << "\n\n"; 
		std::cout << "DATA : " << postData << "\n\n";
		// postData = "";
		// postData = "answer=thisismyanswer";
 		
		std::string env_vars[] = {
			"REDIRECT_STATUS=200",
			"REQUEST_METHOD=POST",
			"QUERY_STRING=" + postData,
			"AUTH_TYPE=",
			"CONTENT_TYPE=application/x-www-form-urlencoded",
			"GATEWAY_INTERFACE=CGI/1.1",
			"SERVER_PROTOCOL=HTTP/1.1",
			"SERVER_SOFTWARE=webserv/1.0",
			"CONTENT_LENGTH=" + intToString(postData.length()),
			"REMOTE_USER=",
			"REMOTE_IDENT=",
			"SERVER_PORT=8080",
			"SERVER_NAME=cgi",
			"REMOTE_ADDR=127.0.0.1",
			"REMOTE_HOST=127.0.0.1:8080",
			"SERVER_PROTOCOL=HTTP/1.1",
			"SERVER_SOFTWARE=webserv/1.0",
	
			"SCRIPT_FILENAME=Websites/website1/default/cgi-bin/test.php",
			"PATH_TRANSLATED=Websites/website1/cgi/cgi-bin/test.php",
			"SCRIPT_NAME=/cgi-bin/test.php",
			"REQUEST_URI=/cgi-bin/test.php",
			"PATH_INFO=/cgi-bin/test.php"
		};

		std::cout << "BEFORE\n";
		char *new_env[sizeof(env_vars) / sizeof(env_vars[0])];
		for (size_t i = 0; i < sizeof(env_vars) / sizeof(env_vars[0]) - 1; i++) {
			new_env[i] = new char[env_vars[i].size() + 1];
			strcpy(new_env[i], env_vars[i].c_str());
		}
		new_env[sizeof(env_vars) / sizeof(env_vars[0]) - 1] = NULL;
	
		if (pipe(pipes) == -1) {

			// error
		}

		write(pipes[1], postData.c_str(), postData.length());

		get_cgi(script_path, CGI, CGI_PATH);

		// Check if the file was opened successfully
		if (!tmpFile) {
			//std::cout << "Failed to open the output file." << std::endl;
			return "";
		}

		// Command to execute the PHP script
		// const char* command[] = {CGI.c_str(), "-f", script_path.c_str()};
		const char* command[] = {CGI.c_str(), "-f", script_path.c_str()};

		// Fork a child process
		pid_t work_pid = fork();

		if (work_pid == -1) {
			std::cerr << "Failed to fork a child process." << std::endl;
			return "";
		} 
		// Work child process
		else if (work_pid == 0) {
			
			close(pipes[1]);

			// Redirect the output to the file
			if (dup2(fileno(tmpFile), STDOUT_FILENO) == -1)
				exit(EXIT_FAILURE);
			if (dup2(pipes[0], STDIN_FILENO) == -1)
				exit(EXIT_FAILURE);

			// Execute the PHP script
			execve(CGI_PATH.c_str(), const_cast<char**>(command), new_env);
			// execve(CGI_PATH.c_str(), const_cast<char**>(command), NULL);
			
			// If execve returns, an error occurred
			std::cerr << "Failed to execute the PHP script." << std::endl;
			exit(EXIT_FAILURE);	
			// return "";
		}

		pid_t timeout_pid = fork();
		if (timeout_pid == -1) {
			std::cerr << "Failed to fork a child process." << std::endl;
			return "";
		}
		else if (timeout_pid == 0) {
			sleep(30); // In seconds
			_exit(0);
		}

		pid_t exited_pid = wait(NULL);
		if (exited_pid == work_pid)
			kill(timeout_pid, SIGKILL);
		else if (exited_pid == timeout_pid) {
			std::cerr << "PHP script execution timed out." << std::endl;
			kill(work_pid, SIGKILL); // Or something less violent if you prefer
		}
		wait(NULL); // Collect the other process

	

		close(pipes[0]);
		close(pipes[1]);
		fclose(tmpFile);
		
		std::ifstream rFile("tmpFile.txt");
		while (!rFile.eof()) {
			std::string tmp = "";
			rFile >> tmp;
			std::cout << tmp;
			
			// std::cout << "APPENDING\n";
			body.append(tmp);
		}
		
		// std::string line;
		// while (std::getline(rFile, line)) {
		// 	std::cout << line;
		// 	body.append(line);
		// }
			
		rFile.close();

		// UNCOMMENT 
		// int result = remove("tmpFile.txt");


		if (body.find("UTF-8") != std::string::npos)
			body = body.substr(body.find("UTF-8") + 5);
		return body;
	}

// Needed ?
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
// --------------