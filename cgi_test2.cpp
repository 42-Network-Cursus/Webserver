#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Includes/constantes.hpp"
#include <fstream>

// INTO UTILS



int main() {
	std::cout << get_body_from_cgi("Websites/cgi/cgi-bin/script.php") << std::endl;
}
