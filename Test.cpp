#include <vector>
#include <string>
#include <iostream>

int main() {
	std::string msg = "test";

	std::vector<std::string> vec;

	vec.push_back(msg);

	vec[0] = "not a test";
	vec.erase(vec.begin());

	std::cout << "msg " << msg << std::endl;
	// std::cout << "vec " << vec[0] << std::endl;
}