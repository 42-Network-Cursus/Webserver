#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

const int PORT = 3490;
const std::string RESPONSE = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><head><title>Hello, World!</title></head><body><h1>Hello, World!</h1></body></html>";

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    sockaddr_in serverAddress = {};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Failed to bind socket to port " << PORT << "." << std::endl;
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 1) == -1) {
        std::cerr << "Failed to listen on port " << PORT << "." << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Server is listening on http://localhost:" << PORT << std::endl;

    while (true) {
        sockaddr_in clientAddress = {};
        socklen_t clientAddressSize = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressSize);
        if (clientSocket == -1) {
            std::cerr << "Failed to accept connection." << std::endl;
            close(serverSocket);
            return 1;
        }

        char requestBuffer[1024];
        memset(requestBuffer, 0, sizeof(requestBuffer));
        ssize_t bytesRead = recv(clientSocket, requestBuffer, sizeof(requestBuffer) - 1, 0);
        if (bytesRead == -1) {
            std::cerr << "Failed to read data from client." << std::endl;
            close(clientSocket);
            close(serverSocket);
            return 1;
        }

        std::cout << "Received request:\n" << requestBuffer << std::endl;

        ssize_t bytesSent = send(clientSocket, RESPONSE.c_str(), RESPONSE.length(), 0);
        if (bytesSent == -1) {
            std::cerr << "Failed to send response to client." << std::endl;
        } else {
            std::cout << "Sent response:\n" << RESPONSE << std::endl;
        }

        close(clientSocket);
    }

    close(serverSocket);
    return 0;
}