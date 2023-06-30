#include "Server.hpp"

Client* Server::searchClient(int clientFD) {
    for (std::vector<Client>::iterator it = this->clients.begin(); it != this->clients.end(); ++it) {
        if (it->getFD() == clientFD) {
            return &(*it);  // nee jort het kan niet anders
        }
    }
    return NULL;
}

bool Server::isNicknameInUse(const std::string& nickname) const {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i].getNick() == nickname) {
            return true;
        }
    }
    return false;
}

void Server::createClient(int newSocket) {
    pollfd newFD;
    newFD.fd = newSocket;
    newFD.events = POLLIN;
    newFD.revents = 0;
    this->getFDS().push_back(newFD);
    Client newClient(newSocket);
    this->getClients().push_back(newClient);
}