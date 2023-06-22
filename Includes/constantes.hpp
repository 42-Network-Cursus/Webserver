#ifndef CONSTANTES_HPP

# define CONSTANTES_HPP

#include <string>

/* STATUS CODE*/

const std::string SC_UNKNOWN = "Code Unknown";

const std::string SC_200 = "OK";

const std::string SC_400 = "Bad Request";
const std::string SC_403 = "Forbidden";
const std::string SC_404 = "Not Found";
const std::string SC_418 = "I'm a teapot";

const std::string SC_500 = "Internal Server Error";


/* ERROR MESSAGE */

const std::string ERR_CONVERSION_ITS = "Error: Conversion int to string failed";

const std::string INVALID_HEADER = "Invalid Header";
#endif