#include "Server.hpp"

void Server::processActivity() {
    for (size_t i = 0; i < this->fds.size(); i++) {
        if (this->fds[i].revents & POLLIN) {          // check if there is data to read in fd
            if (this->fds[i].fd == this->serverFD) {  // if there is new data to read on serverFD -> new client
                acceptNewConnection();
            } else {
                handleClient(i);  // else its from an existing client
            }
        }
    }
}

void Server::sendMessages() {
    message msg;
    
    size_t nMessages = this->messages.size();

    for (size_t i = 0; i < nMessages; i++) {
        msg = this->messages.front();
        this->sendMessage(msg.first, msg.second);
        this->messages.pop();
    }
}

void Server::run() {
    try {
        while (true) {
            int activity = poll(&(this->fds[0]), this->fds.size(), -1);

            if (activity < 0) {
                throw std::runtime_error("Server Error: Error while polling");
            }

            processActivity();
            sendMessages();
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}

// TODO: Make sure that server frees all of its client when it exits