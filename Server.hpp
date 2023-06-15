#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <vector>
#include <fcntl.h>
#include "Client.hpp"

class Server {
   private:
    struct sockaddr_in address;
    std::vector<pollfd> fds;
    int server_fd;

    int max_clients;
    std::vector<Client> clients;

    const int port;
    const std::string password;

   public:
    Server(int port, std::string password);
    void run();

    class ServerException : public std::exception {
       private:
        std::string message;

       public:
        explicit ServerException(const std::string& errorMessage) : message("Server Error: " + errorMessage) {}

        ~ServerException() throw() {}

        const char* what() const throw() { return message.c_str(); }
    };
};

#endif