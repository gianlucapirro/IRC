#ifndef UTILS_HPP
#define UTILS_HPP

#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <sstream>

#include "Client.hpp"

int setNonBlocking(int fd);
void sendPrivateMessage(Client *client, std::vector<Client*> *clients, const std::vector<std::string>& args, std::queue<message>* messageQueue);
Client *getClientByNick(std::vector<Client*>* clients, std::string nick);
std::vector<std::string> splitString(const std::string& str, char delimiter);
void removeStrFromVec(std::vector<std::string>& vec, std::string str);

#endif
