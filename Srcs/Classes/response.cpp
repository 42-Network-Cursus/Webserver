#include "response.hpp"

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

/* *************************************************************** */
/* ************************** HTML METHOD ************************ */
/* *************************************************************** */

/**
 * @brief Get something
 * 
 */
void	Response::getMethod()
{

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
void	Response::deleteMethod()
{

}

/* *************************************************************** */
/* ************************** HTML METHOD ************************ */
/* *************************************************************** */

std::string Response::getResponseInString()
{
	std::string response = _header.transformHeaderToString(418, "text/html", "112", "", "", "") + _body;

	return response;
}

/* *************************************************************** */
/* ************************** Setter ****************************** */
/* *************************************************************** */

void Response::setBody(const std::string &body)
{
	_body = body;
}