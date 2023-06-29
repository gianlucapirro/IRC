#include <string>

class Client {
   private:
    int fd;
    std::string nick;
    std::string username;

   public:
    Client(int fd);

    int getFD() const;
    const std::string& getNick() const;
    const std::string& getUsername() const;

    void setNick(const std::string& nick);
    void setUsername(const std::string& username);

    bool isValidNick(const std::string& nick);
};
