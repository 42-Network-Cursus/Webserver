#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Includes/constantes.hpp"
#include <fstream>

int main() {
 
	FILE *tmpFile = fopen("tmpFile.txt", "w");
	std::string msg;

    // Check if the file was opened successfully
    if (!tmpFile) {
        std::cout << "Failed to open the output file." << std::endl;
        return 1;
    }

    // Command to execute the PHP script
    const char* command[] = {PHP_CGI.c_str(), "-f", "html/cgi/script.php", NULL};

    // Fork a child process
    pid_t pid = fork();

    if (pid == -1) {
        std::cerr << "Failed to fork a child process." << std::endl;
        return 1;
    } 
    // Child process
	else if (pid == 0) {

        // Redirect the output to the file
        dup2(fileno(tmpFile), STDOUT_FILENO);

        // Execute the PHP script
        execve(PHP_CGI_PATH.c_str(), const_cast<char**>(command), NULL);
        
        // If execve returns, an error occurred
        std::cerr << "Failed to execute the PHP script." << std::endl;
        return 1;
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
			msg.append(tmp);
		}
			
		rFile.close();
		int result = remove("tmpFile.txt");
		std::cout << result << std::endl;

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            std::cout << "HTML output has been generated." << std::endl;
        } else {
            std::cerr << "PHP script execution failed." << std::endl;
            return 1;
        }
    }

	std::cout << msg;


    return 0;
}
