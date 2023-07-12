#include "cgi.hpp"

CGI::CGI()
{
	_execPath = "";
}

CGI::~CGI()
{

}

CGI::CGI(const CGI &copy) : _execPath(copy._execPath), _env(copy._env)
{

}

CGI &CGI::operator=(const CGI &other)
{
	if (this != &other)
	{
		this->_execPath = other._execPath;
		this->_env = other._env;
	}
	return (*this);
}


CGI::CGI(const std::string &execPath)
{
	_execPath = execPath;
}

void CGI::setEnv(std::string type, std::string length, std::string method, std::string query, std::string root, std::string pathInfo, std::string redirect)
{
	_env["CONTENT_TYPE"] = type;
	_env["CONTENT_LENGTH"] = length;
	_env["REQUEST_METHOD"] = method;
	_env["QUERY_STRING"] = query;
	_env["ROOT"] = root;
	_env["PATH_INFO"] = pathInfo;
	_env["REDIRECT_STATUS"] = redirect;
}


std::string CGI::execute(std::string path)
{
	pid_t pid;

	int p_in[2];
	int p_out[2];

	char *args[] = {
		const_cast<char *>(_execPath.c_str()),
		const_cast<char *>(path.c_str()),
		0
	};
	char **env = _env.c_array();
	if (pipe(p_in) == 1)
		return errorPage();
	if (pipe(p_out) == -1)
	{
		close(p_in[0]);
		close(p_in[1]);
		return errorPage();
	}

	pid_t pid = fork();
	if (pid == -1)
	{
		close(p_in[0]);
		close(p_in[1]);
		close(p_out[0]);
		close(p_out[1]);
		return errorPage();
	}
	if (pid == 0)
	{
		if (dup2(p_in[0], STDIN_FILENO) == -1)
			return errorPage();
		if (dup2(p_out[1], STDOUT_FILENO) == -1)
			return errorPage();
		close(p_in[1]);
		close(p_out[0]);
		execve(args[0], args, env);
		exit(EXIT_FAILURE);
	}
	close(p_in[0]);
	close(p_out[1]);
	if (write(p_in[1], "Salut\n", 6) == -1)
	{
		close(p_in[1]);
		close(p_out[0]);
		return errorPage();
	}
	close(p_in[1]);
	int status;
	waitpid(pid, &status, WNOHANG);
	if (WEXITSTATUS(status))
	{
		close(p_out[0]);
		return errorPage();
	}
	std::string res;
	int size = 1024;
	char buff[size];
	size_t nb;
	nb = read(p_out[0], buff, size);
	while (nb == size)
	{
		res.append(buff, nb);
		nb = read(p_out[0], buff, size);
	}
	close(p_out[0]);
	if (nb < 0)
		return errorPage();
	res.append(buff, nb);

	return (res);
}

std::string CGI::errorPage()
{
	return "<!DOCTYPE html>\n<html><title>Error Page... Nique Ta Mere Salope</title><body><h1>It's a basic error page.</h1><h5>Don't read the title.</h5></body></html>\n";
}