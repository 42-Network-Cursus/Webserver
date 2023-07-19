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

std::string get_body_from_cgi(std::string script, char *env) {
 
	FILE 		*tmpFile = fopen("tmpFile.txt", "w");
	std::string body;
	std::string script_path = trim(script);
	std::string CGI;
	std::string CGI_PATH;


	std::string env_vars[2];
	
	env_vars[0] = "CONTENT_LENGTH=0";
	// env_vars[1] = "CONTENT_TYPE=" + request.headers["Content-Type"];
	// env_vars[1] = "CONTENT_TYPE=text/html";
	// env_vars[2] = "REQUEST_METHOD=POST";
	// env_vars[3] = "REDIRECT_STATUS=200";
	// env_vars[4] = "GATEWAY_INTERFACE=CGI/1.1";
	// env_vars[5] = "SCRIPT_NAME=";
	// env_vars[6] = "PATH_INFO=";
	// env_vars[7] = "PATH_TRANSLATED=";
	// env_vars[8] = "REMOTE_ADDR=" + request.headers["Origin"];
	// env_vars[9] = "AUTH_TYPE=" + request.headers["Authorization"];
	// env_vars[10] = "QUERY_STRING=" + request.query_string; // set only for get requests
	// env_vars[11] = "REMOTE_USER=" + request.headers["Authorization"];
	env_vars[1] = "answer=THISISANSWER";
	// char *tmp[1];
	//  = {"answer=THISISANSWER", ""};
	// tmp[0] = str;

	char *new_env[3];

	int i = 0;
	while (i < 2) {
		new_env[i] = new char[env_vars[i].size() + 1];
		strcpy(new_env[i], env_vars[i].c_str());
		++i;
	}
	new_env[i] = NULL;

	// env = tmp;
	std::cout << "IN CGI \n\n";

	get_cgi(script_path, CGI, CGI_PATH);

    // Check if the file was opened successfully
    if (!tmpFile) {
        //std::cout << "Failed to open the output file." << std::endl;
        return "";
    }

    // Command to execute the PHP script
    const char* command[] = {CGI.c_str(), "-f", script_path.c_str(), NULL};

    // Fork a child process
    pid_t work_pid = fork();

    if (work_pid == -1) {
        std::cerr << "Failed to fork a child process." << std::endl;
        return "";
    } 
    // Work child process
	else if (work_pid == 0) {

        // Redirect the output to the file
        if ( (dup2(fileno(tmpFile), STDOUT_FILENO)) == -1)
			exit(EXIT_FAILURE);

        // Execute the PHP script
        execve(CGI_PATH.c_str(), const_cast<char**>(command), new_env);
        
        // If execve returns, an error occurred
        std::cerr << "Failed to execute the PHP script." << std::endl;
		exit(EXIT_FAILURE);	
        return "";
    }

	pid_t timeout_pid = fork();
	if (timeout_pid == -1) {
        std::cerr << "Failed to fork a child process." << std::endl;
        return "";
    }
	else if (timeout_pid == 0) {
		sleep(5); // In seconds
		_exit(0);
	}

	pid_t exited_pid = wait(NULL);
    if (exited_pid == work_pid)
        kill(timeout_pid, SIGKILL);
    else {
		std::cerr << "PHP script execution timed out." << std::endl;
        kill(work_pid, SIGKILL); // Or something less violent if you prefer
	}
    wait(NULL); // Collect the other process

    // Parent process
	// else {

    //     // Wait for the child process to finish
    //     int status;
    //     waitpid(work_pid, &status, 0);

		fclose(tmpFile);
		
		std::ifstream rFile("tmpFile.txt");
		while (!rFile.eof()) {
			std::string tmp;
			rFile >> tmp;
			body.append(tmp);
		}
			
		rFile.close();
		// UNCOMMENT 
		// int result = remove("tmpFile.txt");


		//std::cout << result << std::endl;

        // if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        //     //std::cout << "HTML output has been generated." << std::endl;
        // } else {
        //     std::cerr << "PHP script execution failed." << std::endl;
        //     return "";
        // }
    // }

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