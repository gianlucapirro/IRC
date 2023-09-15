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

#include "Client.hpp"

int setNonBlocking(int fd);
Client *getClientByNick(std::vector<Client*>* clients, std::string nick);

#endif
