#include <string>

class Client {
   private:
    int fd;
    bool isAuthenticated;
    std::string nick;
    std::string username;
    std::string hostname;

   public:
    Client(int fd);

    int getFD() const;
    const std::string& getNick() const;
    const std::string& getUsername() const;
    const std::string& getHostname() const;
    const std::string getFullClientIdentifier() const;
    bool getIsAuthenticated() const;

    void setNick(const std::string& nick);
    void setUsername(const std::string& username);
    void setHostname(const std::string& hostname);
    void setIsAuthenticated(bool isAuthenticated);

    bool isValidNickname(const std::string& nick);
    bool isValidUsername(const std::string& user);
};
