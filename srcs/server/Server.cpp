#include "Server.hpp"

void Server::processActivity(char* buffer) {
    for (size_t i = 0; i < this->fds.size(); i++) {
        if (this->fds[i].revents & POLLIN) {          // check if there is data to read in fd
            if (this->fds[i].fd == this->serverFD) {  // if there is new data to read on serverFD -> new client
                std::cout << "new connection" << std::endl;
                acceptNewConnection();
            } else {
                std::cout << "no new connection" << std::endl;
                handleClient(i, buffer);  // else its from an existing client
            }
        }
    }
    std::cout << "END OF HANDLE PROCCESS" << std::endl;
}

void Server::run() {
    while (true) {
        std::cout << "HELLO WORLD" << std::endl;
        try {
            int activity = poll(&(this->fds[0]), this->fds.size(), -1);
            std::cout << activity << std::endl;
            if (activity < 0) {
                throw std::runtime_error("Server Error: Error while polling");
            }

            char buffer[BUFFER_SIZE] = {0};
            processActivity(buffer);
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            continue;
        }
    }
    std::cout << "END" << std::endl;
}
