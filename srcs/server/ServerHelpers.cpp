#include "Server.hpp"
#include "utils.hpp"

void Server::createClient(int newSocket) {
    pollfd newFD;
	setNonBlocking(newSocket);
    newFD.fd = newSocket;
    newFD.events = POLLIN;
    newFD.revents = 0;
    this->getFDS().push_back(newFD);
    Client* newClient = new Client(newSocket);
    this->getClients().push_back(newClient);
}


Client* Server::searchClient(int clientFD) {
    std::vector<Client*> clients = this->getClients();
    for (size_t i = 0; i < clients.size(); i++) {
        if (clients[i]->getFD() == clientFD) {
            return clients[i];
        }
    }

    return NULL;
}