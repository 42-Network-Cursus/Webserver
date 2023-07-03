#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <string>
# include <utility>
# include <map>

class Location {
    public:

        // SETTERS
        void setPath(std::string val);
        void setRoot(std::string val);
        void setIndex(std::string val);
        void setClientMaxBodySize(std::string val);
        void setMethod(std::string method, bool val);

        // GETTERS
        std::string getPath();
        std::string getRoot();
        std::string getIndex();
        std::string getClientMaxBodySize();
        bool getMethod(std::string method);

    private:
        std::string path;
        std::string root;
        std::string index;
        std::string client_max_body_size;
        std::map<std::string, bool> methods; 
       
        // methods
        // dir_listing
        // cgi
};

#endif // LOCATION_HPP