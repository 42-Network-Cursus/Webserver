#include "location.hpp"

// SETTERS
void Location::setPath(std::string val) {
    path = val;
}

void Location::setRoot(std::string val) {
    root = val;
}

void Location::setIndex(std::string val) {
    index = val;
}

void Location::setClientMaxBodySize(std::string val) {
    client_max_body_size = val;
}


// GETTERS
std::string Location::getPath() {
    return path;
}

std::string Location::getRoot() {
    return root;
}

std::string Location::getIndex() {
    return index;
}

std::string Location::getClientMaxBodySize() {
    return client_max_body_size;
}

