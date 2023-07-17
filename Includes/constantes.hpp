#ifndef CONSTANTES_HPP
# define CONSTANTES_HPP

# include <string>

/*		ACCEPTED METHODS	*/

const std::string METHOD_GET = "GET";
const std::string METHOD_POST = "POST";
const std::string METHOD_DELETE = "DELETE";

/*		STATUS CODE			*/

const std::string SC_UNKNOWN = "Code Unknown";

const std::string SC_200 = "OK";
const std::string SC_201 = "Created";
const std::string SC_204 = "No Content";

const std::string SC_400 = "Bad Request";
const std::string SC_403 = "Forbidden";
const std::string SC_404 = "Not Found";
const std::string SC_413 = "Request Entity Too Large";
const std::string SC_418 = "I'm a teapot";

const std::string SC_500 = "Internal Server Error";
const std::string SC_501 = "Not Implemented";
const std::string SC_505 = "HTTP Version not supported";


/*		Content Type		*/
/*	Les normes -> https://www.iana.org/assignments/media-types/media-types.xhtml		*/

const std::string CT_HTML = "text/html";
const std::string CT_CSS = "text/css";
const std::string CT_JS = "text/javascript";
const std::string CT_PNG = "image/png";
const std::string CT_ICON = "image/x-icon";
const std::string CT_DEFAULT = "text/plain";

const std::string CT_MULTI = "multipart/form-data;";
/*		CGI PATH			*/

const std::string CGI_PHP = "/cgi/cgi.php";
const std::string CGI_PY = "/cgi/cgi.py";
const std::string CGI_UNKNOWN = "Unknown CGI";


/*		ERROR MESSAGE		*/

const std::string ERR_CONVERSION_ITS = "Error: Conversion int to string failed";

const std::string INVALID_HEADER = "Invalid Header";

/*			   CGI			*/

const std::string PHP_CGI_PATH = "/Users/cwastche/.brew/Cellar/php/8.2.8/bin/php-cgi";
const std::string PHP_CGI = "php-cgi";

// IF WE CAN ADD IT
// const char PY_CGI_PATH[] = ""; 
// const char PY_CGI[] = "";

#endif