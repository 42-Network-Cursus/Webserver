#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <string>
# include <map>

class Location {
    public:
        Location();
        ~Location();
		Location(const Location &copy);
		Location &operator=(const Location &rhs);

    // SETTERS
        void    setPath(std::string val);
        void    setRoot(std::string val);
        void    setIndex(std::string val);
        void    setClientMaxBodySize(std::string val); 
        void    setErrorPagePath(std::string val);
        void    setScriptPath(std::string val);
        void    setUploadPath(std::string val);
        void    setAutoIndex(std::string val);
        void    setMethod(std::string method, bool val);
        void    setHostPort(std::string val);
        void    setRedirect(std::string val);

    // GETTERS
        std::string                 getPath() const;
        std::string                 getRoot() const;
        std::string                 getIndex() const;
        std::string                 getClientMaxBodySize() const;
        std::string                 getErrorPagePath();
        std::string                 getScriptPath() const;
        std::string                 getUploadPath() const;
        bool                        getAutoIndex() const;
        std::map<std::string, bool> getMethods() const;
        bool                        getMethod(std::string method);
        std::string                 getHostPort() const;
        std::string                 getRedirect() const;
		
	// DEBUG
        void print();
    
	private:
        std::string                 _path;
        std::string                 _root;
        std::string                 _index;
        std::string                 _client_max_body_size;
        std::string                 _error_page_path;
        std::string                 _script_path;
        std::string                 _upload_path;
        bool                        _auto_index;
        std::map<std::string, bool> _methods;
        std::string                 _host_port;
        std::string                 _redirect;
};

#endif // LOCATION_HPP