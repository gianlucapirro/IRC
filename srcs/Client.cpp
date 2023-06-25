#include "Client.hpp"

Client::Client(int fd, const std::string& password) : fd(fd), password(password), nick(""), username("") {}

int Client::getFD() const { return fd; }
const std::string& Client::getPassword() const { return password; }
const std::string& Client::getNick() const { return nick; }
const std::string& Client::getUsername() const { return username; }

void Client::setNick(const std::string& nick) { this->nick = nick; }
void Client::setUsername(const std::string& username) { this->username = username; }
