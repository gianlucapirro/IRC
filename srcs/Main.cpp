#include "Config.hpp"
#include "Server.hpp"

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) {
            std::cerr << "Invalid arguments, please use as: ./ircserv <port> <password>" << std::endl;
            exit(EXIT_FAILURE);
        }

        Config config(argv[1], argv[2]);
        Server server(config);
        server.run();
    } catch (const std::runtime_error& ex) {
        std::cerr << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}