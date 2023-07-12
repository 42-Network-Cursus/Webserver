#include <iostream>
#include <fstream>
#include <cstdlib>

int main() {
    // Create a temporary file
    std::ofstream outputFile("temp.html");

    // Check if the file was opened successfully
    if (!outputFile) {
        std::cerr << "Failed to open the output file." << std::endl;
        return 1;
    }

    // Command to execute the PHP script
    std::string command = "php-cgi -f script.php";

    // Open a pipe to execute the command
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "Failed to open pipe." << std::endl;
        return 1;
    }

    // Read the output of the PHP script and write it to the temporary file
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        outputFile << buffer;
    }

    // Close the pipe and the output file
    pclose(pipe);
    outputFile.close();

    std::cout << "HTML output has been written to temp.html" << std::endl;

    return 0;
}

