#ifndef SERVER_HPP
#define SERVER_HPP

#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <vector>

#include 

class ChannelHandler {
   private:

    std::vector<pollfd> channels;      // stores all sockets fd

    void setupServer();
    void createSocket();
    void setSocketOptions(int opt);
    void configureAddress();
    void bindSocket();
    void setListenMode();
    void processActivity();
    void acceptNewConnection();
    void createClient(int newSocket);
    void handleClient(size_t i);

   public:
    Server(const Config& config);

    void handleBuffer(char* buffer, int valread, int fd);

    Client* searchClient(int clientFD);
    void sendMessage(int clientFD, const std::string& message);
    bool isNicknameInUse(const std::string& nickname) const;

    const Config& getConfig() const;
    std::vector<pollfd>& getFDS();
    std::vector<Client>& getClients();
    void run();
};

#endif
