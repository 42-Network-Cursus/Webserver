#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <string>
# include <utility>
# include <map>

class Location {

    public:
        
        Location();

        // SETTERS
        void setPath(std::string val);
        void setRoot(std::string val);
        void setIndex(std::string val);
        void setClientMaxBodySize(std::string val); 
        void setErrorPagePath(std::string val);
        void setCgiPath(std::string val);
        void setUploadPath(std::string val);
        void setAutoIndex(std::string val);
        void setMethod(std::string method, bool val);

        // GETTERS
        std::string getPath();
        std::string getRoot();
        std::string getIndex();
        std::string getClientMaxBodySize();
        std::string getErrorPagePath();
        std::string getCgiPath();
        std::string getUploadPath();
        bool        getAutoIndex();
        bool        getMethod(std::string method);

    private:
        std::string                 _path;
        std::string                 _root;
        std::string                 _index;
        std::string                 _client_max_body_size;
        std::string                 _error_page_path;
        std::string                 _cgi_path;
        std::string                 _upload_path;
        bool                        _auto_index;
        std::map<std::string, bool> _methods;

};

#endif // LOCATION_HPP