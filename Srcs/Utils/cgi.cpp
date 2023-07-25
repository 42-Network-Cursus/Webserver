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

std::string url_encode(const std::string value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : value) {
        // Keep alphanumeric and other safe characters unchanged
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            // Encode other characters as %XX
            escaped << '%' << std::setw(2) << static_cast<unsigned>(static_cast<unsigned char>(c));
        }
    }

    return escaped.str();
}

	std::string get_body_from_cgi(std::string script, std::string postData) {
	
		FILE 		*tmpFile = fopen("tmpFile.txt", "w");
		std::string body = "";
		std::string script_path = trim(script);
		std::string CGI;
		std::string CGI_PATH;
		int pipes[2];

		std::string encodedQueryString = url_encode("name=John&age=30");
		std::string query_string = "QUERY_STRING=" + encodedQueryString;
		std::string request_method = "REQUEST_METHOD=GET";
		

		// force post
		postData = "answer=thisismyanswer";

		std::cout << "\n IN CGI \n\n";

		if (!postData.empty()) {	
			encodedQueryString = postData;
			// url_encode(postData);
			// query_string = "QUERY_STRING=" + encodedQueryString;
			request_method = "REQUEST_METHOD=POST";
		}
 		
		std::string env_vars[] = {
			"CONTENT_LENGTH=" + std::to_string(postData.length()),
			query_string,
			request_method,
			"REDIRECT_STATUS=200",
			"CONTENT_TYPE=application/x-www-form-urlencoded",
			"SCRIPT_NAME=/test.php",
			"GATEWAY_INTERFACE=CGI/1.1"
			// Websites/website1/default
			// Websites/website1/default/cgi-bin/
		};

		std::cout << "BEFORE\n";
		char *new_env[sizeof(env_vars) / sizeof(env_vars[0])];
		for (size_t i = 0; i < sizeof(env_vars) / sizeof(env_vars[0]) - 1; i++) {
			new_env[i] = new char[env_vars[i].size() + 1];
			strcpy(new_env[i], env_vars[i].c_str());
		}
		new_env[sizeof(env_vars) / sizeof(env_vars[0]) - 1] = NULL;
		
		int i = 0;
		while (i < 4) {
			std::cout << new_env[i];
			i++;
		}
		std::cout << "AFTER\n";
	
	
		if (pipe(pipes) == -1) {

			// error
		}

		write(pipes[1], postData.c_str(), postData.length());


		// std::string env_vars[5];
		
		// GET
		// env_vars[0] = "";
		// // "CONTENT_LENGTH=" + std::to_string(encodedQueryString.length());
		// env_vars[1] =  "QUERY_STRING=" + encodedQueryString; // We don't need the CONTENT_TYPE for GET requests
		// env_vars[2] = "REQUEST_METHOD=GET"; // Use GET method for passing parameters in the URL
		// env_vars[3] = "REDIRECT_STATUS=200";
		// env_vars[4] = "CONTENT_TYPE=application/x-www-form-urlencoded";
		
		
		// POST
		// env_vars[0] = "CONTENT_LENGTH=" + std::to_string(postData.length());
		// env_vars[1] = "";
		// // "CONTENT_TYPE=application/x-www-form-urlencoded";
		// //  + request.headers["Content-Type"];
		// // env_vars[1] = "CONTENT_TYPE=text/html";
		// env_vars[2] = "REQUEST_METHOD=POST";
		// env_vars[3] = "REDIRECT_STATUS=200";
		// env_vars[4] = "PATH_INFO=" + script_path;
		

		// char *new_env[6];
		// //  = new char;

		// int i = 0;
		// while (i < 6) {

		// 	new_env[i] = new char[env_vars[i].size() + 1];
		// 	strcpy(new_env[i], env_vars[i].c_str());
		// 	++i;
		// 	// std::cout << new_env[i] << std::endl;
		// }
		// new_env[i] = NULL;

	// 	std::string query = "QUERY_STRING=" + encodedQueryString;
	// 	 char *new_env[] = {
    //     (char *)query.c_str(),
    //     (char *)"REQUEST_METHOD=GET",
    //     (char *)"REDIRECT_STATUS=200",
    //     NULL
    // };

		// env = tmp;
		// std::cout << "IN CGI \n\n";

		get_cgi(script_path, CGI, CGI_PATH);

		// Check if the file was opened successfully
		if (!tmpFile) {
			//std::cout << "Failed to open the output file." << std::endl;
			return "";
		}

		// Command to execute the PHP script
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
			if ( (dup2(fileno(tmpFile), STDOUT_FILENO) || dup2(pipes[0], STDIN_FILENO) == -1) == -1) // 
				exit(EXIT_FAILURE);

			// Execute the PHP script
			
			execve(CGI_PATH.c_str(), const_cast<char**>(command), new_env);
			// execve(CGI_PATH.c_str(), const_cast<char**>(command), NULL);
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

			close(pipes[0]);
			close(pipes[1]);
			fclose(tmpFile);
			
			std::ifstream rFile("tmpFile.txt");
			while (!rFile.eof()) {
				std::string tmp = "";
				rFile >> tmp;
				std::cout << tmp << std::endl;
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