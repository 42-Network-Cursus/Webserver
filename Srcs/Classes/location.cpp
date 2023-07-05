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

void Location::setAutoIndex(bool val) {
    _auto_index = val;
}

void Location::setMethod(std::string method, bool val) {
    _methods.insert(std::make_pair(method, val));
}


// GETTERS
std::string Location::getPath() {
    return _path;
}

std::string Location::getRoot() {
    return _root;
}

std::string Location::getIndex() {
    return _index;
}

std::string Location::getClientMaxBodySize() {
    return _client_max_body_size;
}

std::string Location::getErrorPagePath() {
    return _error_page_path;
}

std::string Location::getCgiPath() {
    return _cgi_path;
}

std::string Location::getUploadPath() {
    return _upload_path;
}

bool        Location::getAutoIndex() {
    return _auto_index;
}

bool Location::getMethod(std::string method) {
    return _methods[method];
}

