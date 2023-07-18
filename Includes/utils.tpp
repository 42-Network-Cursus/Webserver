#ifndef UTILS_TPP
# define UTILS_TPP

# include "utils.hpp"

	template<typename T>
	bool containsValue(const std::vector<T>& vect, const T& value)
	{
		//std::cout << "Contains value" << std::endl;
		typename std::vector<T>::const_iterator it;
		typename std::vector<T>::const_iterator end;

		it = vect.begin();
		end = vect.end();
		//std::cout << "Heelo => " << *it << std::endl;
		while (it != end)
		{
			//std::cout << *it << " " << value << std::endl;
			if (*it == value)
				return true;
			++it;
		}
		//std::cout << "False !!!" << std::endl;
		return false;
	}

#endif