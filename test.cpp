#include <string>
#include <iostream>

int main()
{
	std::string str = "test.tmp";


	size_t pos = str.find_last_of(".");
	if (pos <= 0)
		std::cout << "ALED" << std::endl;
	std::string res = str.substr(pos + 1);

	std::cout << res << std::endl;
	
	return (0);
}