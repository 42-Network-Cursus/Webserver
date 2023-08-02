#include <fstream>
#include <signal.h>
#include <string.h>

#include "utils.hpp"
#include "request.hpp"

void get_cgi(std::string script_path, std::string &CGI, std::string &CGI_PATH) {
	std::string suffix = script_path.substr(script_path.find_last_of("."));

	if (suffix == ".php") {
		CGI = PHP_CGI;
		CGI_PATH = PHP_CGI_PATH;
	}
}

std::pair<std::string, int> get_body_from_cgi(Request &request) {

	FILE 		*tmpFile = fopen("tmpFile.txt", "w");
	std::pair<std::string, int> r_pair;

	r_pair.first = "";
	r_pair.second = 500;

	// Check if the file was opened successfully
	if (!tmpFile) { 
		r_pair.first = "Failed to open the output file.";
		return r_pair;
	}

	std::string body = "";
	std::string CGI;
	std::string CGI_PATH;
	int 		pipes[2];
	
	std::string script_path = request.getLocationConfig().getScriptPath();
	script_path = trim(script_path);
	std::string script_name = script_path.substr(script_path.find_last_of("/", script_path.find_last_of("/") - 1));
	script_name = trim(script_name);

	std::string host_port = request.getLocationConfig().getHostPort();
	std::string data = "";

	if (request.getMethod() == METHOD_GET)
		data = request.getQuery();
	if (request.getMethod() == METHOD_POST)
		data = request.getBody();

	std::string env_vars[] = {
		"REDIRECT_STATUS=200",
		"REQUEST_METHOD=" + request.getMethod(),
		"QUERY_STRING=" + data,
		"AUTH_TYPE=",
		"CONTENT_TYPE=application/x-www-form-urlencoded",
		"GATEWAY_INTERFACE=CGI/1.1",
		"SERVER_PROTOCOL=" + request.getVersion(),
		"SERVER_SOFTWARE=webserv/1.0",
		"CONTENT_LENGTH=" + intToString(data.length()),
		"REMOTE_USER=",
		"REMOTE_IDENT=",
		"SERVER_PORT=" + host_port.substr(host_port.find_first_of(":") + 1, host_port.find_first_of(";") - host_port.find_first_of(":")),
		"SERVER_NAME=" + request.getServerName(),
		"REMOTE_ADDR=" + host_port.substr(0, host_port.find_first_of(":")),
		"REMOTE_HOST=" + host_port,
		"SCRIPT_FILENAME=" + script_path,
		"SCRIPT_NAME=" + script_name
	};
	
	char *new_env[18];
	for (size_t i = 0; i < 17; i++) {
		new_env[i] = new char[env_vars[i].size() + 1];
		strcpy(new_env[i], env_vars[i].c_str());
	}
	new_env[17] = NULL;

	get_cgi(script_path, CGI, CGI_PATH);

	if (pipe(pipes) == -1) {
		r_pair.first = "Failed to open pipes.";
		return r_pair;
	}

	write(pipes[1], data.c_str(), data.length());
	
	// Command to execute the PHP script
	const char* command[] = {CGI.c_str(), "-f", script_path.c_str(), NULL};

	pid_t work_pid = fork();
	if (work_pid == -1) {
		r_pair.first = "Failed to fork a child process.";
		return r_pair;
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
		
		// If execve returns, an error occurred
		std::cerr << "Failed to execute the PHP script." << std::endl;
		exit(EXIT_FAILURE);
	}

	pid_t timeout_pid = fork();
	if (timeout_pid == -1) {
		r_pair.first = "Failed to fork a child process.";
		return r_pair;
	}
	else if (timeout_pid == 0) {
		sleep(5); // In seconds
		_exit(0);
	}

	pid_t exited_pid = wait(NULL);
	if (exited_pid == work_pid)
		kill(timeout_pid, SIGKILL);
	else if (exited_pid == timeout_pid) {
		r_pair.first = "PHP script execution timed out.";
		kill(work_pid, SIGKILL);
		return r_pair;
	}
	wait(NULL); // Collect the other process

	close(pipes[0]);
	close(pipes[1]);
	fclose(tmpFile);
	
	std::ifstream rFile("tmpFile.txt");
	while (!rFile.eof()) {
		std::string tmp = "";
		rFile >> tmp;
		body.append(tmp);
	}

	rFile.close();
	std::remove("tmpFile.txt");

	if (body.find("UTF-8") != std::string::npos)
		body = body.substr(body.find("UTF-8") + 5);
	if (body.find("Warning") != std::string::npos)
		body = body.substr(0, body.find("Warning"));
		
	if (body.find("Parseerror") != std::string::npos) {
		r_pair.first = "PHP Script syntax error";
		return r_pair;
	}
	r_pair.first = body;
	r_pair.second = 200;
	return r_pair;
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
