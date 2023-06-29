#include "Server.hpp"

Client& Server::searchClient(int clientFD) {
    for (std::vector<Client>::iterator it = this->clients.begin(); it != this->clients.end(); ++it) {
        if (it->getFD() == clientFD) {
            return *it;
        }
    }
    throw std::runtime_error("Server Error: Client not found.");
}

bool Server::isNicknameInUse(const std::string& nickname) const {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i].getNick() == nickname) {
            return true;
        }
    }
    return false;
}