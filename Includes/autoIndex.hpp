#ifndef AUTOINDEX_HPP
# define AUTOINDEX_HPP

# include <string>
# include <dirent.h>
# include <iostream>

std::string generateAutoIndex(std::string path, std::string host, std::string port);

# endif