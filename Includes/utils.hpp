#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>

std::string& ltrim(std::string& s, const char* t = " \t\r\f\v");
std::string& rtrim(std::string& s, const char* t = " \t\r\f\v");
std::string& trim(std::string& s, const char* t = " \t\r\f\v");

bool skip_line(std::string line);

#endif // UTILS_HPP