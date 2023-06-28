#include <string>

class Client {
   public:
    Client(int fd);

    int getFD() const;
    const std::string& getNick() const;
    const std::string& getUsername() const;

    void setNick(const std::string& nick);
    void setUsername(const std::string& username);

   private:
    int fd;
    std::string nick;
    std::string username;
};
