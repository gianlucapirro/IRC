#include "Config.hpp"

#include <cstdlib>

int Config::convertToInt(const char* port) {
    try {
        return atoi(port);
    } catch (const std::exception& ex) {
        throw std::runtime_error("Config Error: " + std::string(ex.what()));
    }
}

std::string Config::convertToString(const char* password) {
    try {
        if (password == NULL) {
            throw std::runtime_error("Null password provided");
        }
        return std::string(password);
    } catch (const std::exception& ex) {
        throw std::runtime_error("Config Error: " + std::string(ex.what()));
    }
}

Config::Config(char* port, char* password)
    : port(convertToInt(port)), password(convertToString(password)), maxClients(10) {
}

int Config::getPort() const {
    return this->port;
}

int Config::getMaxClients() const {
    return this->maxClients;
}

const std::string& Config::getPassword() const {
    return this->password;
}

Config::~Config() {
}