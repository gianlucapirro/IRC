#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>


class Client {
   private:
    int fd;
    std::string buffer;

    bool isAuthenticated;
	bool isRegistered;
    bool isDeleted;
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
    std::string& getBuffer();
    bool getIsAuthenticated() const;
    void setNick(const std::string& nick);
    void setUsername(const std::string& username);
    void setHostname(const std::string& hostname);
	void registerClient();
	
    void setIsAuthenticated(bool isAuthenticated);
	bool canBeRegistered() const;
    bool getIsRegistered() const;

    bool isValidNickname(const std::string& nick);
    bool isValidUsername(const std::string& user);

    void handleIncomingData(const char* data, size_t length, std::vector<std::string> &commands);
    void del();
    bool getDeleted();
};

#endif