#include "response_header.class.hpp"

/* ********************************************************************************************* */
/* ***************************************** CANONIC FORM ************************************** */
/* ********************************************************************************************* */

ResponseHeader::ResponseHeader()
{
	initValues();
}

ResponseHeader::~ResponseHeader()
{

}

ResponseHeader::ResponseHeader(const ResponseHeader &copy) : _contentType(copy._contentType), _contentLength(copy._contentLength), _date(copy._date), _lastModified(copy._lastModified), _server(copy._server), _location(copy._location)
{

}

ResponseHeader &ResponseHeader::operator=(const ResponseHeader &other)
{
	if (this != &other)
		setValues(other._contentType, other._contentLength, other._date, other._lastModified, other._location);
	return (*this);
}

/* ********************************************************************************************* */
/* ***************************************** SETTER ******************************************** */
/* ********************************************************************************************* */

void ResponseHeader::setContentType(const std::string &type)
{
	this->_contentType = type;
}

void ResponseHeader::setContentLength(const std::string &length)
{
	this->_contentLength = length;
}

void ResponseHeader::setDate(const std::string &date)
{
	this->_date = date;
}

void ResponseHeader::setLastModified(const std::string &date)
{
	this->_lastModified = date;
}

void ResponseHeader::setServer(const std::string &server)
{
	this->_server = server;
}

void ResponseHeader::setLocation(const std::string &location)
{
	this->_location = location;
}

/* ********************************************************************************************* */
/* ***************************************** METHOD ******************************************** */
/* ********************************************************************************************* */

/**
 * @brief Transform header datas into a std::string
 * 
 * 			Maybe put arguments like type, length, etc into a new Object
 * 
 * @param statusCode Status Code
 * @param type 
 * @param length 
 * @param date 
 * @param last 
 * @param location 
 * 
 * @return Header data in std::string
 */
std::string ResponseHeader::transformHeaderToString(int statusCode, std::string type, std::string length, std::string date, std::string last, std::string location)
{
	std::string header;

	std::string codeText = getTextByStatusCode(statusCode);

	std::string code = intToString(statusCode);
	if (code == SC_UNKNOWN)
		return INVALID_HEADER;

	initValues();
	setValues(type, length, date, last, location);
	header = "HTTP/1.1 " + code + " " + codeText + "\r\n";
	header += getHeaderData();
	return header;
}

std::string ResponseHeader::getHeaderData()
{
	std::string res = "";

	if (this->_contentType != "")
		res += "Content-Type: " + this->_contentType + "\r\n";
	if (this->_contentLength != "")
		res += "Content-Length: " + this->_contentLength + "\r\n";
	if (this->_date != "")
		res += "Date: " + this->_date + "\r\n";
	if (this->_lastModified != "")
		res += "Last-Modified: " + this->_lastModified + "\r\n";
	if (this->_location != "")
		res += "Location: " + this->_location + "\r\n";
	if (this->_server != "")
		res += "Server: " + this->_server + "\r\n";

	res += "\r\n";
	return res;
}

void ResponseHeader::initValues()
{
	this->_contentType = "";
	this->_contentLength = "";
	this->_date = "";
	this->_lastModified = "";
	this->_server = "Webserv";
	this->_location = "";
}

void ResponseHeader::setValues(const std::string &type, const std::string &length, const std::string &date, const std::string &last, const std::string &location)
{
	setContentType(type);
	setContentLength(length);
	setDate(date);
	setLastModified(last);
	setLocation(location);
}