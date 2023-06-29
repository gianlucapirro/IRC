#include "Client.hpp"

#include <iostream>

Client::Client(int fd) : fd(fd), nick(""), username("") {}

// getters
int Client::getFD() const { return fd; }

const std::string& Client::getNick() const { return this->nick; }

const std::string& Client::getUsername() const { return this->username; }

const std::string& Client::getHostname() const { return this->hostname; }

const std::string Client::getFullClientIdentifier() const {
    return "<" + this->nick + ">!<" + this->username + ">@<" + this->hostname + ">";
};

bool Client::getIsAuthenticated() const { return this->isAuthenticated; }

// setters
void Client::setIsAuthenticated(bool isAuthenticated) { this->isAuthenticated = isAuthenticated; }

void Client::setNick(const std::string& nick) { this->nick = nick; }

void Client::setUsername(const std::string& username) { this->username = username; }

void Client::setHostname(const std::string& hostname) { this->hostname = hostname; }

// validation
bool Client::isValidNickname(const std::string& nick) {
    if (nick.empty()) return false;
    if (!isalpha(nick[0])) return false;  // Check if the first character is a letter

    // Check the remaining characters
    for (size_t i = 1; i < nick.size(); ++i) {
        if (!isalnum(nick[i]) && nick[i] != '-' && nick[i] != '[' && nick[i] != ']' && nick[i] != '\\' &&
            nick[i] != '`' && nick[i] != '^' && nick[i] != '{' && nick[i] != '}') {
            return false;
        }
    }

    return true;
}

bool Client::isValidUsername(const std::string& username) {
    if (username.empty()) return false;

    for (size_t i = 0; i < username.size(); ++i) {
        if (username[i] == '\0' ||  // NUL
            username[i] == '\r' ||  // CR
            username[i] == '\n' ||  // LF
            username[i] == ' ' ||   // Space
            username[i] == '@' || username[i] == ':') {
            return false;
        }
    }

    return true;
}
