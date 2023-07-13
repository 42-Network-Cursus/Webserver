#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Includes/constantes.hpp"
#include <fstream>

std::string get_body_from_cgi() {
 
	FILE *tmpFile = fopen("tmpFile.txt", "w");
	std::string body; // RETURN VALUE
	std::string script_path =  "html/cgi/script.php"; // ARGUMENT

	// GET CORRECT CONSTANT FROM script_path SUFFIX
	std::string CGI = PHP_CGI;
	std::string CGI_PATH = PHP_CGI_PATH;

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

int main() {
	std::cout << get_body_from_cgi() << std::endl;
}
