#include <string>
#include <cctype>
#include <sstream>
#include "Config.hpp"
#include "Server.hpp"

bool isValidPort(const std::string& port) {
    if (port.empty()) {
        return false;
    }

    for (std::string::const_iterator it = port.begin(); it != port.end(); ++it) {
        if (!std::isdigit(*it)) {
            return false;
        }
    }

    int portNumber;
    std::istringstream iss(port);
    iss >> portNumber;

    if (!iss || portNumber <= 0 || portNumber > 65535 || port[0] == '0') {
        return false;
    }

    return true;
}


bool isValidPassword(const std::string& password) {
    if (password.empty())
        return false;
    //check if pass is letter or number
    for (std::string::const_iterator it = password.begin(); it != password.end(); ++it) {
        if (!std::isalnum(*it))
            return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) {
            std::cerr << "Invalid arguments, please use as: ./ircserv <port> <password>" << std::endl;
            exit(EXIT_FAILURE);
        }

        if (!isValidPort(argv[1])) {
            std::cerr << "Invalid port, please use a port between 1 and 65535" << std::endl;
            exit(EXIT_FAILURE);
        }

        if (!isValidPassword(argv[2])) {
            std::cerr << "Invalid password, please use a password" << std::endl;
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