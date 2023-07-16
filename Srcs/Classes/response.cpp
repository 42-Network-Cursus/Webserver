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
		_statusCode = 403; // Or 405 ?
	// 	_statusCode = 501;
	// else if (_statusCode == 200 && request.isValidVersion() == false)
	// 	_statusCode = 505;
	// else if (_statusCode == 200 && request.isValidPath() == false)
	// 	_statusCode = 404;
	
	#ifdef DEBUG
	std::cout << "\n\n Status Code => " << _statusCode << std::endl;
	#endif
	
	if (_statusCode != 200)
	{
		_path = "Websites/errorPage/" + intToString(_statusCode) + "_page.html";
		readFile();
		_path = "";
		_header = ResponseHeader();
		_header.setContentType(CT_HTML);
		_header.setContentLength(intToString(_body.size()));
	}
	else if (request.getMethod() == METHOD_GET)
		getMethod(request);
	else if (request.getMethod() == METHOD_POST)
		postMethod(request);
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

	std::cout << "IN GET METHOD " << _path << std::endl;

	if (_path == request.getLocationConfig().getPath()) {
		
		if (request.getLocationConfig().getScriptPath() != "") {
			
			_body = get_body_from_cgi(request.getLocationConfig().getScriptPath());
			
			// Can't really return, need to handle if script fails and send an error page
			return;
		}
		else
			_path = request.getDefaultPage();
	}
	readFile();
	if (_statusCode == 404)
	{
		_path = "Websites/errorPage/" + intToString(_statusCode) + "_page.html";
		readFile();
		_path = "";
		_header = ResponseHeader();
		_header.setContentType(CT_HTML);
		_header.setContentLength(intToString(_body.size()));
	}
	
}

/**
 * @brief Post something
 * 
 */
void	Response::postMethod(Request request)
{
	// std::cout << "------------------------------ In POSTMETHOD" << std::endl;
	_path = request.getPath();
	std::string ext = getExtension(_path);
	// std::cout << "POSTMETHOD:\n\nContentType: " << request.getContentType() << std::endl;
	if (isCGIExtension(ext))
	{
		std::string cgi = getCGIbyExtension(ext);
		// std::cout << "On emploie CGI" << std::endl;
		return ;
	}
	else if (true || request.getContentType() == CT_MULTI) // A changer !!!
	{
		// Un truc... Encore en train de tout setup
		// std::cout << "=============================================================== On va créer le fichier" << std::endl;
		if (checkUploadPath(request.getUploadPath()) == false)
		{
			_statusCode = 500;
			_path = "html/errorPage/" + intToString(_statusCode) + "_page.html";
			readFile();
			_path = "";
			_header = ResponseHeader();
			_header.setContentType(CT_HTML);
			_header.setContentLength(intToString(_body.size()));
			return ;
		}
		// if (request.getUploadPath() != request.getPath())
		// 	_path = request.getUploadPath() + request.getPath();
		writeFile(request.getBody());
	}
	// std::cout << "On post !" << std::endl;
	request.setPath("/");
	getMethod(request);
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
		std::cout << "NOT VALID PATH ??" << std::endl;

		_statusCode = 404;
		_path = "Websites/errorPage/" + intToString(_statusCode) + "_page.html";
		readFile();
		_path = "";
		return ;
	}



	std::ifstream file;
	std::stringstream buffer;

	file.open(_path.c_str(), std::ifstream::in);
	if (file.is_open() == false)
	{
		_statusCode = 404;
		_path = "Websites/errorPage/" + intToString(_statusCode) + "_page.html";
			readFile();
			_path = "";
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
	// std::cout << "On tente de creer le fichier" << std::endl;
	std::ofstream file;

	if (isValidPathFile() == false)
	{
		file.open(_path.c_str(), std::ofstream::out | std::ofstream::trunc);
		if (file.is_open() == false)
		{
			_statusCode = 403;
			_path = "html/errorPage/" + intToString(_statusCode) + "_page.html";
			readFile();
			_path = "";
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
	std::cout << _path << std::endl;
	if (access(_path.c_str(), F_OK) != 0 || access(_path.c_str(), R_OK) != 0)
		return false;
	// DIR *dir = opendir(_path.c_str());
	// if (dir != NULL)
	// 	return false;
	// closedir(dir);
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



bool Response::checkUploadPath(std::string path)
{
	path = deleteWhiteSpace(path);
	// std::cout << "--------------------------------------------------------------- path directory: " << path << std::endl;
	DIR* dir = opendir(path.c_str());
	if (dir != NULL)
	{
		closedir(dir);
		return true;
	}
	return false;
}