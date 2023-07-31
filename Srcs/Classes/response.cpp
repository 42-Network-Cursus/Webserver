#include "response.hpp"

// DEBUG
void Response::print() {
	//std::cout << "\n\nResponse info: " << std::endl;
	//std::cout << "Status code: " << _statusCode << std::endl;
	_header.print();
	//std::cout << "Body : " << _body << std::endl;
	//std::cout << "Path : " << _path << std::endl;
}

Response::Response() {}

Response::~Response() {}

Response::Response(const Response &copy)
: _statusCode(copy._statusCode), _header(copy._header), _body(copy._body), _path(copy._path), _filename(copy._filename)
{}

Response &Response::operator=(const Response &other)
{
	if (this != &other)
	{
		this->_statusCode = other._statusCode;
		this->_header = other._header;
		this->_body = other._body;
		this->_path = other._path;
		this->_filename = other._filename;
	}
	return (*this);
}

Response::Response(Request request)
{
	if (request.getMethod() == REQ_INV)
	{
		std::cout << "Invalid request." << std::endl;
		_statusCode = 400;
		generateError();
		return ;
	}
	_statusCode = 200;
	_filename = "";

	if (request.isAcceptedMethod() == false) {
		std::cout << "Unauthorized method." << std::endl;
		_statusCode = 501;
	}
	if (_statusCode == 200 && request.isValidVersion() == false)
		_statusCode = 505;
	if (_statusCode == 200)
		_statusCode = request.isValidPath();

	if (_statusCode != 200 && _statusCode != 301)
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

	if (_path == request.getLocationConfig().getPath()) {
		
		if (request.getLocationConfig().getScriptPath() != "") {
			_body = get_body_from_cgi(request);
			
			_path = request.getLocationConfig().getScriptPath(); // For content type handling (Uses suffix)
			_header.setContentLength(intToString(_body.length()));
			return ;
		}
		else
		{
			if (request.getLocationConfig().getAutoIndex())
			{
				_path = request.getLocationConfig().getRoot();
				_path = trim(_path);
				_body = generateAutoIndex(_path);
				_filename = "autoindex.html";
				
				return ;
			}
			else	
				_path = request.getDefaultPage();
		}
	}
	
	else
	{
		size_t pos = _path.find("/"); 
		size_t pos2 = _path.find("/", pos + 1);
		if (pos2 == std::string::npos)
		{
			if (_path[0] == '/')
			{
				std::string root = request.getLocationConfig().getRoot();
				root = trim(root);
				_path = root + _path;
			}
		}
		else
		{
			std::string tmp = _path.substr(pos2 + 1);
			std::string root = request.getLocationConfig().getRoot();
			root = trim(root);
			_path = root + tmp;
		}
	}
	readFile();
	if (_statusCode == 404)
		generateError();
}

/**
 * @brief Post something
 * 
 */
void	Response::postMethod(Request request)
{
	_path = request.getPath();
	std::string ext = getExtension(_path);

	if (isCGIExtension(ext))
	{	
		request.getLocationConfig().setScriptPath(request.getLocationConfig().getRoot() + "cgi-bin" + request.getPath());
		request.getLocationConfig().setPath(_path);
	}
	else if (isFile(request.getContentType()))
	{
		int maxSize = sint(request.getLocationConfig().getClientMaxBodySize());
		if (request.getContentSize() > maxSize)
		{
			_statusCode = 413;
			generateError();
			return ;
		}
		_filename = getFilename(request.getBody());
		std::string fileContent = getContentBody(request.getBody());
		writeFile(_filename, fileContent);
		_body = getUploadedFilePage();
		_filename = "upload.html";
		return ;
	}
	getMethod(request);
}


/**
 * @brief Delete something
 * 
 */
void	Response::deleteMethod(Request request)
{
	_path = request.getPath();
	_path = request.getLocationConfig().getRoot() + _path;

	if (_path == request.getLocationConfig().getPath())
		_path = request.getDefaultPage();

	if (isValidPathFile() == false)
	{
		_statusCode = 404;
		generateError();
		return ;
	}
	
	if (std::remove(_path.c_str()) == 0)
	{
		_statusCode = 204;
		_body = getDeletedFilePage();
		_filename = "deleted.html";
	}
	else
	{
		_statusCode = 404;
		generateError();
	}
}

/* *************************************************************** */
/* ************************** HTML METHOD ************************ */
/* *************************************************************** */

std::string Response::getResponseInString()
{
	std::string type;
	
	if (_statusCode > 400)
		type = _header.getContentType();
	else
	{
		if (_filename != "")
			type = getExtensionFile(_filename);
		else
			type = getExtension(_path);
			
		if (type == "")
			type = CT_DEFAULT;
		else
			type = getContentType(type);
	}

	std::string response = _header.transformHeaderToString(_statusCode, type, intToString(_body.size()), "", "", "") + _body;
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
		generateError();
		return ;
	}

	std::ifstream file;
	std::stringstream buffer;

	file.open(_path.c_str(), std::ifstream::in);
	if (file.is_open() == false)
	{
		_statusCode = 404;
		generateError();
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
void	Response::writeFile(std::string filename, std::string content)
{
	std::ofstream 	file;
	std::string		filepath = "Websites/upload/" + filename;
	
	file.open(filepath.c_str(), std::ofstream::out | std::ofstream::trunc);
	if (file.is_open() == false)
	{
		_statusCode = 403;
		generateError();
	}

	file << content;
	file.close();
	_statusCode = 201;
}

/**
 * @brief Checks if the path is valid or not
 * 
 */
/*		V1		*/
bool Response::isValidPathFile()
{
	_path = trim(_path);
	
	if (access(_path.c_str(), F_OK) != 0)
		return false;
	
	if (access(_path.c_str(), R_OK) != 0)
		return false;

	if (checkUploadPath(_path))
		return false;

	return true;
}

/**
 * @brief Generate the error page
 * 
 * @return Error page in HTML 
 */
std::string Response::getDeletedFilePage()
{
	return "<!DOCTYPE html>\n<html><title>Deleted file</title><body><h1>Deleted file</h1></body></html>\n";
}

std::string Response::getUploadedFilePage()
{
	return "<!DOCTYPE html>\n<html><title>Uploaded file</title><body><h1>Uploaded file</h1></body></html>\n";
}

bool Response::checkUploadPath(std::string path)
{
	path = deleteWhiteSpace(path);
	DIR* dir = opendir(path.c_str());
	if (dir != NULL)
	{
		closedir(dir);
		return true;
	}
	return false;
}

void Response::generateError()
{
	_path = "Websites/errorPage/" + intToString(_statusCode) + "_page.html";
	readFile();
	_path = "";
	_header = ResponseHeader();
	_header.setContentType(CT_HTML);
	_header.setContentLength(intToString(_body.size()));
}

int	Response::getStatusCode() {
	return _statusCode;
}