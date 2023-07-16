#include <iostream>
#include <fstream>

#include "utils.hpp"

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
	std::string script_path = trim(script);
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
        
		// exit(EXIT_FAILURE);
		
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