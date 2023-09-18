#include "utils.hpp"
#include "ResponseBuilder.hpp"
#include "CommandHandler.hpp"

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

void sendPrivateMessage(Client *client, std::vector<Client*> *clients, const std::vector<std::string>& args, std::queue<message>* messageQueue) {
    if (args.size() < 2) {
        std::string response = ResponseBuilder("ircserv").addCommand("461").addTrailing("Not enough parameters").build();
        messageQueue->push(std::make_pair(client->getFD(), response));
        return;
    }

    std::stringstream ss(args[0]);
    std::string user;
    while (std::getline(ss, user, ',')) {
        Client* receiver = getClientByNick(clients, user);
        if (receiver) {
            std::string response = ":" + client->getNick() + " PRIVMSG";
            std::ostringstream oss;
            for(size_t i = 1; i < args.size(); ++i) { // Start from 1 to skip user list
                oss << " ";
                oss << args[i];
            }
            response += " " + user + oss.str(); // Add user
            response += "\r\n";
            messageQueue->push(std::make_pair(receiver->getFD(), response));
        }
    }
}

void removeStrFromVec(std::vector<std::string>& vec, std::string str) {
    for (size_t i = 0; i < vec.size(); i++) {
        if (vec[i] == str) {
            vec.erase(vec.begin() + i);
            return;
        }
    }
}