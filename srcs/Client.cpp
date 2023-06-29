#include "Client.hpp"

#include <iostream>

Client::Client(int fd) : fd(fd), nick(""), username("") {}

int Client::getFD() const { return fd; }

const std::string& Client::getNick() const { return nick; }

const std::string& Client::getUsername() const { return username; }

void Client::setNick(const std::string& nick) { this->nick = nick; }

void Client::setUsername(const std::string& username) { this->username = username; }

bool Client::isValidNick(const std::string& nick) {
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