#include "Server.hpp"


void Server::createClient(int newSocket) {
    pollfd newFD;
    newFD.fd = newSocket;
    newFD.events = POLLIN;
    newFD.revents = 0;
    this->getFDS().push_back(newFD);
    Client* newClient = new Client(newSocket);
    this->getClients().push_back(newClient);
}