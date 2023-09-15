#include "utils.hpp"

int setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
		return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}


Client *getClientByNick(std::vector<Client*>* clients, std::string nick) {
    for (size_t i = 0; i < clients->size(); i++) {
        Client* client = (*clients)[i];
        if (client->getNick() == nick) {
            return client;
        }
    }

    return NULL;
}

std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);

    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}
