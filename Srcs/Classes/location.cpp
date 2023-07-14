#include "location.hpp"
#include <iostream>
#include "utils.hpp"

// Debugging
void Location::print() {
    std::cout << "Path: " << _path << std::endl;
    std::cout << "	root: " << _root << std::endl;
    std::cout << "	index: " << _index << std::endl;
    std::cout << "	client_max_body_size: " << _client_max_body_size << std::endl;
    std::cout << "	error page path: " << _error_page_path << std::endl;
    std::cout << "	script path: " << _script_path << std::endl;
    std::cout << "	upload path: " << _upload_path << std::endl;
    std::cout << "	auto index: " << _auto_index << std::endl;
    std::cout << "	Methods: " << std::endl;
    std::cout << "		GET: " << getMethod("GET") << std::endl;
    std::cout << "		POST: " << getMethod("POST") << std::endl;
    std::cout << "		DELETE: " << getMethod("DELETE") << std::endl;
}

// CONSTRUCTORS

Location::Location() 
: _auto_index(false)
{}

Location::~Location() {
}

Location::Location(const Location &copy) {

    _path                   = copy.getPath();
    _root                   = copy.getRoot();
    _index                  = copy.getIndex();
    _client_max_body_size   = copy.getClientMaxBodySize();
    _error_page_path        = copy.getErrorPagePath();
    _script_path               = copy.getScriptPath();
    _upload_path            = copy.getUploadPath();
    _auto_index             = copy.getAutoIndex();
    _methods                = copy.getMethods();
    
}

Location &Location::operator=(const Location &rhs) {
    if (this != &rhs) {
         _path                   = rhs.getPath();
        _root                   = rhs.getRoot();
        _index                  = rhs.getIndex();
        _client_max_body_size   = rhs.getClientMaxBodySize();
        _error_page_path        = rhs.getErrorPagePath();
        _script_path               = rhs.getScriptPath();
        _upload_path            = rhs.getUploadPath();
        _auto_index             = rhs.getAutoIndex();
        _methods                = rhs.getMethods();
    }
    return *this;
}


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

void Location::setScriptPath(std::string val) {
    _script_path = val;
}

void Location::setUploadPath(std::string val) {
    _upload_path = val;
}

void Location::setAutoIndex(std::string val) {
    val = trim(val);
    if (val == "on")
        _auto_index = true;
}

void Location::setMethod(std::string method, bool val) {
    _methods.insert(std::make_pair(method, val));
}


// GETTERS
std::string                 Location::getPath() const {
    return _path;
}

std::string                 Location::getRoot() const {
    return _root;
}

std::string                 Location::getIndex() const {
    return _index;
}

std::string                 Location::getClientMaxBodySize() const {
    return _client_max_body_size;
}

std::string                 Location::getErrorPagePath() const {
    return _error_page_path;
}

std::string                 Location::getScriptPath() const {
    return _script_path;
}

std::string                 Location::getUploadPath() const {
    return _upload_path;
}

bool                        Location::getAutoIndex() const {
    return _auto_index;
}

std::map<std::string, bool> Location::getMethods() const {
    return _methods;
}

bool                        Location::getMethod(std::string method) {
    return _methods[method];
}

