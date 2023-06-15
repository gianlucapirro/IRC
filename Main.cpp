#include "Server.hpp"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Invalid arguments, please use as: ./ircserv <port> <passoword>" << std::endl;
        exit(EXIT_FAILURE);
    }
    int port = std::stoi(argv[1]);
    std::string password(argv[2]);
    Server server(port, password);
    server.run();
    return 0;
}
