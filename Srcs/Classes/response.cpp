#include "response.hpp"
#include "request.hpp"

// DEBUG
void Response::print() {
	std::cout << "\n\nResponse info: " << std::endl;
	std::cout << "Status code: " << _statusCode << std::endl;
	_header.print();
	std::cout << "Body : " << _body << std::endl;
	std::cout << "Path : " << _path << std::endl;
}

Response::Response()
{

}

Response::~Response()
{

}

Response::Response(const Response &copy) : _statusCode(copy._statusCode), _header(copy._header), _body(copy._body), _path(copy._path)
{

}

Response &Response::operator=(const Response &other)
{
	if (this != &other)
	{
		this->_statusCode = other._statusCode;
		this->_header = other._header;
		this->_body = other._body;
		this->_path = other._path;
	}
	return (*this);
}

Response::Response(Request request)
{

	_statusCode = 200;

	if (request.isAcceptedMethod() == false)
		_statusCode = 501;
	else if (_statusCode == 200 && request.isValidVersion() == false)
		_statusCode = 505;
	else if (_statusCode == 200 && request.isValidPath() == false)
		_statusCode = 404;
	
	#ifdef DEBUG
	std::cout << "\n\n Status Code => " << _statusCode << std::endl;
	#endif
	
	
	if (_statusCode != 200)
	{
		_body = getPageErrorStatus(_statusCode);
		_path = "";
		_header = ResponseHeader();
		_header.setContentType(CT_HTML);
		_header.setContentLength(intToString(_body.size()));
	}
	else if (request.getMethod() == METHOD_GET)
		getMethod(request);
	else if (request.getMethod() == METHOD_POST)
		postMethod();
	else if (request.getMethod() == METHOD_DELETE)
		deleteMethod(request);
}

/* *************************************************************** */
/* ************************** HTML METHOD ************************ */
/* *************************************************************** */

/**
 * @brief Get something
 * 
 */
void	Response::getMethod(Request request)
{
	_path = request.getPath();
	readFile();
	if (_statusCode == 404)
		_body = getErrorPage();
	_body = "<!DOCTYPE html><html><head><title>NTM</title></head><body><h1>On va briser des os...</h1></body></html>";
	std::cout << "End Get Method" << std::endl;
}

/**
 * @brief Post something
 * 
 */
void	Response::postMethod()
{

}

/**
 * @brief Not really need
 * 
 */
void	Response::putMethod()
{

}

/**
 * @brief Delete something
 * 
 */
void	Response::deleteMethod(Request request)
{
	_path = request.getPath();
	if (isValidPathFile() == false)
	{
		_statusCode = 404;
		return ;
	}
	if (std::remove(_path.c_str()) == 0)
	{
		_statusCode = 204;
		_body = "";
	}
	else
	{
		_statusCode = 404;
		_body = getErrorPage();
	}
}

/* *************************************************************** */
/* ************************** HTML METHOD ************************ */
/* *************************************************************** */

std::string Response::getResponseInString()
{
	std::string response = _header.transformHeaderToString(200, "text/html", intToString(_body.size()), "", "", "") + _body;

	return response;
}

/* *************************************************************** */
/* ************************** Setter ****************************** */
/* *************************************************************** */

void	Response::setBody(const std::string &body)
{
	_body = body;
}

/**
 * @brief Read the content of a file and modify the status code to 404 if an error occurs
 * 
 */
void	Response::readFile()
{
	if (isValidPathFile() == false)
	{
		_statusCode = 404;
		return ;
	}
	std::ifstream file;
	std::stringstream buffer;

	file.open(_path.c_str(), std::ifstream::in);
	if (file.is_open() == false)
	{
		_statusCode = 404;
		return ;
	}
	buffer << file.rdbuf();
	setBody(buffer.str());
	file.close();
}

/**
 * @brief Writes in a file or creates a new file
 * 
 * @param content Content to write
 */
void	Response::writeFile(std::string content)
{
	std::ofstream file;

	if (isValidPathFile() == false)
	{
		file.open(_path.c_str(), std::ofstream::out | std::ofstream::trunc);
		if (file.is_open() == false)
		{
			_statusCode = 403;
			return ;
		}
		file << content;
		file.close();
		_statusCode = 201;
	}
	else
	{
		file.open(_path.c_str());
		if (file.is_open() == false)
		{
			_statusCode = 403;
			return ;
		}
		file << content;
		file.close();
		_statusCode = 204;
	}
}

/**
 * @brief Checks if the path is valid or not
 * 
 */
/*		V1		*/
bool Response::isValidPathFile()
{
	if (access(_path.c_str(), F_OK) != 0 || access(_path.c_str(), R_OK) != 0)
		return false;
	DIR *dir = opendir(_path.c_str());
	if (dir != NULL)
		return false;
	closedir(dir);
	return true;
}

/*		V2		*/
// bool Response::isValidPathFile()
// {
// 	struct stat s_stat;
// 	if (stat(_path.c_str(), &s_stat))
// 	{
// 		if (s_stat.st_mode & S_IFDIR)
// 			return false;
// 		else if (s_stat.st_mode & S_IFREG)
// 			return true;
// 		else
// 			return false;
// 	}
// 	else
// 		return false;
// }

/**
 * @brief Generate the error page
 * 
 * @return Error page in HTML 
 */
std::string Response::getErrorPage()
{
	return "<!DOCTYPE html>\n<html><title>Error Page... Nique Ta Mere Salope</title><body><h1>It's a basic error page.</h1><h5>Don't read the title.</h5></body></html>\n";
}

std::string Response::getPageErrorStatus(int statusCode)
{
	switch (statusCode)
	{
		case 404:
			return getErrorPage();
		case 501:
			return getErrorPage();
	}
	return "";
}