#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    // Create a temporary file
    FILE* outputFile = fopen("output.txt", "w"); 

    // Check if the file was opened successfully
    if (!outputFile) {
        std::cerr << "Failed to open the output file." << std::endl;
        return 1;
    }

    // Command to execute the PHP script
    const char* command[] = {"php-cgi", "-f", "script.php", nullptr};

    // Fork a child process
    pid_t pid = fork();

    if (pid == -1) {
        std::cerr << "Failed to fork a child process." << std::endl;
        return 1;
    } else if (pid == 0) {
        // Child process

        // Redirect the output to the file
        dup2(fileno(outputFile), STDOUT_FILENO);
                
        // Execute the PHP script
        execve("/root/projects/Webserver/php-cgi", const_cast<char**>(command), nullptr);
        

        // If execve returns, an error occurred
        std::cerr << "Failed to execute the PHP script." << std::endl;
        return 1;
    } else {
        // Parent process

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

        // Close and remove the output file
        fclose(outputFile);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            std::cout << "HTML output has been generated." << std::endl;
        } else {
            std::cerr << "PHP script execution failed." << std::endl;
            return 1;
        }
    }

    return 0;
}
