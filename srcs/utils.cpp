#include "utils.hpp"

int setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
		return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}


Client *getClientByUsername(std::vector<Client*>* clients, std::string name) {
    for (size_t i = 0; i < clients->size(); i++) {
        Client* client = (*clients)[i];
        if (client->getUsername() == name) {
            return client;
        }
    }

    return NULL;
}
