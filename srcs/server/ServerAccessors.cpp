#include "Server.hpp"

const Config& Server::getConfig() const { return this->config; }

std::vector<pollfd>& Server::getFDS() { return this->fds; }
std::vector<Client>& Server::getClients() { return this->clients; }