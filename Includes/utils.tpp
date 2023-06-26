#ifndef UTILS_TPP
# define UTILS_TPP

# include "utils.hpp"

	template<typename T>
	bool containsValue(const std::vector<T>& vect, const T& value)
	{
		typename std::vector<T>::const_iterator it;
		typename std::vector<T>::const_iterator end;

		it = vect.begin();
		end = vect.end();
		while (it != end)
		{
			if (*it == value)
				return true;
			++it;
		}
		return false;
	}

#endif