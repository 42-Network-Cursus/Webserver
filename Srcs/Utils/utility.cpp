#include "webserv.hpp"

/*******************
	Debugging
*******************/ 
void		print_server_list(std::vector<Server> servers) {
	std::vector<Server>::iterator it_begin = servers.begin();
	std::vector<Server>::iterator it_end = servers.end();

	//std::cout << "Number of servers: " << servers.size() << std::endl << std::endl;
	for (; it_begin != it_end; it_begin++) {
		it_begin->print();
		//std::cout << std::endl;
	}
}

/********************
	Utility
********************/

std::string &ltrim(std::string &s, const char* t) { // trim from left
    s.erase(0, s.find_first_not_of(t));
    return s;
}

std::string &rtrim(std::string &s, const char* t) { // trim from right
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

std::string &trim(std::string &s, const char* t) { // trim from left & right
    return ltrim(rtrim(s, t), t);
}

// Skips comments and empty lines in configuration file
bool skip_line(std::string line) {
	if (trim(line)[0] == '#' || line.length() == 0)
		return true;
	return false;
}

bool stringToBool(std::string str) {
	str = trim(str);
	if (str == "true")
		return true;
	return false;
}

/**
 * @brief Get the text corresponding to the status code
 * 
 * @param code Status Code
 * @return Status Code Text
 */
std::string getTextByStatusCode(int code)
{
	switch (code)
	{
		case 200:
			return SC_200;
		case 201:
			return SC_201;
		case 204:
			return SC_204;
		case 301:
			return SC_301;
		case 400:
			return SC_400;
		case 403:
			return SC_403;
		case 404:
			return SC_404;
		case 413:
			return SC_413;
		case 418:
			return SC_418;
		case 500:
			return SC_500;
		case 501:
			return SC_501;
		case 505:
			return SC_505;
		default:
			return SC_UNKNOWN;
	}
}

/**
 * @brief Convert a int to a std::string
 * 
 * @param number Number
 * @return Number value in std::string 
 */
std::string	intToString(int number)
{
	std::string res = "";
	try
	{
		std::ostringstream oss117;
		oss117 << number;
		res = oss117.str();
	}
	catch(const std::exception& e)
	{
		res = ERR_CONVERSION_ITS;
	}
	return res;
}

int sint(std::string str)
{
	std::istringstream iss(str);
    int res;
    iss >> res;

	return (res);
}

std::string deleteWhiteSpace(std::string str)
{
	std::string::const_iterator it = str.begin();
	std::string::const_iterator end = str.end();

	while (std::isspace(*it) && it != end)
		++it;

	std::string res = "";
	while (it != end)
	{
		res += *it;
		++it;
	}

	return res;
}


// bool isInVector(std::vector<std::string> vector, std::string value)
// {
// 	std::vector<std::string>::iterator it = vector.begin();
// 	std::vector<std::string>::iterator end = vector.end();

// 	while(it != end)
// 	{
// 		if (*it == value)
// 			return true;
// 		++it;
// 	}
// 	// //std::cout << "FALSE" << std::endl;
// 	return false;
// }

// void readBody(std::string str)
// {
// 	std::istringstream iss(str);
// 	std::string line;

// 	int i = 0;

// 	while (std::getline(iss, line) && i < 2)
// 	{
// 		//std::cout << line << std::endl;
// 		i++;
// 	}
// }

bool isFile(std::string str)
{
	size_t pos = str.find(CT_MULTI);
	//std::cout << "POS? : " << pos << std::endl;
	if (pos == std::string::npos)
	{
		//std::cout << "Pas du tout" << std::endl;
		return false;
	}
	//std::cout << "On est bon !" << std::endl;
	return true;
}

std::string getFilename(std::string body)
{
	std::istringstream iss(body);
	std::string line;

	std::string content = "Content-Disposition:";
	size_t contentSize = content.size();
	
	std::string filename = "";
	std::string tmp;
	while (std::getline(iss, line))
	{
		// //std::cout << "======== Check Content-Length: " << line << std::endl;
		if (line.compare(0, contentSize, content) == 0)
		{
			size_t pos = line.find("filename=");
			if (pos == std::string::npos)
				return (filename);
			filename = line.substr(pos + 9);
			size_t start = filename.find("\"");
			size_t end = filename.rfind("\"");
			if (start != std::string::npos && end != std::string::npos && start < end)
				filename = filename.substr(start + 1, end - start - 1);
			return filename;
		}
	}
	return (filename);
}

/**
 * @brief Get the Content Body object
 * 
 * @param body Body of the request
 * @return Content of the body
 */
// std::string getContentBody(std::string body)
// {
// 	//std::cout << "In getContentBody\nBody: \n" << body << std::endl;
// 	std::istringstream iss(body);
// 	std::string line;
	
	
// 	std::string tmp;
// 	std::string delimiter;
// 	std::getline(iss, line);
// 	delimiter = trim(line);
// 	//std::cout << "Delimiter: " << delimiter << std::endl;
// 	//std::cout << "First Boucle" << std::endl;
// 	while (std::getline(iss, line))
// 	{
// 		//std::cout << "Line: " << line << " | Size: " << line.size() << std::endl;
// 		// if (line.size() == 2 && line == "\r\n")
// 		// 	break ;
// 		// if (line.size() == 4 && line == "\r\n\r\n")
// 		// 	break;
// 		if (line.size() == 1)
// 			break;
// 	}
// 	//std::cout << "Second Boucle" << std::endl;
// 	std::string res = "";
// 	std::string end = "\n";
// 	// A garder ?
// 	while (std::getline(iss, line))
// 	{	
// 		//std::cout << "Line: " << line << std::endl;
// 		//std::cout << "Deli: " << delimiter << std::endl;
// 		//std::cout << "Check: " << (line.compare(0, delimiter.size(), delimiter.c_str())) << std::endl;
// 		if (line.find(delimiter) != std::string::npos)
// 		{
// 			//std::cout << "On break" << std::endl;
// 			break;
// 		}
// 		//std::cout << "On est passé le check ?" << std::endl;
// 		line = rtrim(line);
// 		// res += line + end;
// 		res += line + end;
// 		//std::cout << "RES in the boucle: " << res << std::endl;
// 	}
// 	//std::cout << "RES: \n" << res << std::endl;
// 	return res;
// }


std::string getContentBody2(std::string body)
{
	size_t start;

	start = body.find("\n");
	std::string delimiter = body.substr(0, start);
	delimiter = trim(delimiter);
	std::cout << delimiter << std::endl;
	size_t end = body.rfind(delimiter);
	start = body.find("\r\n\r\n");
	std::cout << "Start: " << start << " End: " << end << " Size: " << body.size() << std::endl;

	start += 4;
	std::string content = body.substr(start, end - start);
	std::cout << "Content: " << content << std::endl;

	return content;
}

std::string getExtensionFile(std::string str)
{
	//std::cout << "In getEXTENSIONFILE : " << str << std::endl;
	size_t pos = str.find_last_of(".");
	if (pos <= 0)
		return ("");
	std::string res = str.substr(pos + 1);
	return res;
}

std::string getContentType(std::string str)
{
	std::string ext = getExtension(str);
	size_t pos = ext.find_last_of("/");

	if (pos == ext.size() - 1)
	{
		ext.erase(pos);
	}

	if	(ext == "css")
		return CT_CSS;
	else if	(ext == "js")
		return CT_JS;
	else if	(ext == "ico")
		return CT_ICON;
	else if	(ext == "png")
		return CT_PNG;
	else if	(ext == "jpg")
		return CT_JPG;
	else if	(ext == "html" || ext == "php")
		return CT_HTML;
	else if (ext == "mp3")
		return CT_MP3;
	else
		return CT_DEFAULT;
}
