#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <string>

class Location {
    public:

        // SETTERS
        void setPath(std::string val);
        void setRoot(std::string val);
        void setIndex(std::string val);
        void setClientMaxBodySize(std::string val);

        // GETTERS
        std::string getPath();
        std::string getRoot();
        std::string getIndex();
        std::string getClientMaxBodySize();

    private:
        std::string path;
        std::string root;
        std::string index;
        std::string client_max_body_size;
       
        // methods
        // dir_listing
        // cgi
};

#endif // LOCATION_HPP