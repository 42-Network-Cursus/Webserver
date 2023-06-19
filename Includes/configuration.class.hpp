#ifndef CONFIGURATION_CLASS_HPP
# define CONFIGURATION_CLASS_HPP

# include <vector>
# include <fstream>
# include "server.class.hpp"

class Configuration {
	public:
		
		Server&		operator[] (int idx);

		int			size() const;
		void		push_back(const Server& server);
		std::string getBody(int fd);

		// Debugging
		void		print();

	private:
		std::vector<Server>	_server_list;		
};



#endif // CONFIGURATION_CLASS_HPP