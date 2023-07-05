#include "location.hpp"

// CONSTRUCTORS

Location::Location() 
: _auto_index(false)
{}

// SETTERS
void Location::setPath(std::string val) {
    _path = val;
}

void Location::setRoot(std::string val) {
    _root = val;
}

void Location::setIndex(std::string val) {
    _index = val;
}

void Location::setClientMaxBodySize(std::string val) {
    _client_max_body_size = val;
}

void Location::setErrorPagePath(std::string val) {
    _error_page_path = val;
}

void Location::setCgiPath(std::string val) {
    _cgi_path = val;
}

void Location::setUploadPath(std::string val) {
    _upload_path = val;
}

void Location::setAutoIndex(std::string val) {
    if (val == "on")
        _auto_index = true;
    _auto_index = false;
}

void Location::setMethod(std::string method, bool val) {
    _methods.insert(std::make_pair(method, val));
}


// GETTERS
std::string Location::getPath() const {
    return _path;
}

std::string Location::getRoot() const {
    return _root;
}

std::string Location::getIndex() const {
    return _index;
}

std::string Location::getClientMaxBodySize() const {
    return _client_max_body_size;
}

std::string Location::getErrorPagePath() const {
    return _error_page_path;
}

std::string Location::getCgiPath() const {
    return _cgi_path;
}

std::string Location::getUploadPath() const {
    return _upload_path;
}

bool        Location::getAutoIndex() const {
    return _auto_index;
}

bool Location::getMethod(std::string method) {
    return _methods[method];
}

