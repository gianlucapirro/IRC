#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>

#define BUFFER_SIZE 1024

class Config {
   private:
    Config();
    const int port;
    const std::string password;

    static int convertToInt(const char* port);
    static std::string convertToString(const char* password);

   public:
    Config(char* port, char* password);
    ~Config();

    int getPort() const;
    int getBufferSize() const;
    const std::string& getPassword() const;
};

#endif