#include "autoIndex.hpp"

/**
 * @brief Generate the HTML Code oh an autoIndex page
 * 
 * @param path Path of the directory
 * @param host the Host ? (Not directly merge with port ?)
 * @param port the port ? (Not directly merge with host ?)
 * @return std::string empty if can't open the directory, or the html page in std::string
 */
std::string generateAutoIndex(std::string path, std::string host_port)
{
	if (path[0] != '.' && path[1] != '/')
		path = "./" + path;
	DIR *dir = opendir(path.c_str());
	if (dir == NULL)
	{
		//std::cout << path.c_str() << std::endl;
		//std::cout << "Error Open directory in generate AutoIndex" << std::endl;
		////std::cout << strerror(errno) << std::endl;
		return "";
	}
	
	std::string page = "<!DOCTYPE html><html><head><title>" + path + "</title></head><body>";

	struct dirent *entry;
	entry = readdir(dir);

	std::string name;
	while (entry != NULL)
	{
		name = std::string(entry->d_name);
		page += "<p><a href=\"http://" + host_port + path + "/" + name + "\">" + name + "</a></p>";
		entry = readdir(dir);
	}

	page += "</body></html>";
	closedir(dir);

	return (page);
}