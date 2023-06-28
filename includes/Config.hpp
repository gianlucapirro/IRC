#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <string>

#define BUFFER_SIZE 1024

class Config {
   private:
    Config();
    const int port;
    const std::string password;
    const int maxClients;  // max clients that can be in qeuee

    static int convertToInt(const char* port);
    static std::string convertToString(const char* password);

   public:
    Config(char* port, char* password);
    ~Config();

    int getPort() const;
    int getBufferSize() const;
    const std::string& getPassword() const;
    const int getMaxClients() const;
};

#endif