#include "CommandHandler.hpp"

static void handleRegistrationPass(Client* client, const std::vector<std::string>& args, std::string password) {
    if (args[0] == password) {
        client->setIsAuthenticated(true);
    } else {
        respond(client->getFD(), AERR_PASSWDMISMATCH(client->getNick()));
        client->del();
    }
}

static void handleRegistrationNick(Client* client, const std::vector<std::string>& args,
                                   CommandHandler* commandHandler) {
    if (!client->getIsAuthenticated()) {
        respond(client->getFD(), AERR_PASSWDMISMATCH(client->getNick()));
        client->del();
        return;
    }

    if (!client->isValidNickname(args[0])) {
        respond(client->getFD(), AERR_ERRONEUSNICKNAME(client->getNick(), args[0]));
        return;
    }

    if (commandHandler->isNicknameInUse(args[0])) {
        respond(client->getFD(), AERR_NICKNAMEINUSE(std::string("*"), args[0]));
        return;
    }
    client->setNick(args[0]);
}

static void handleRegistrationUser(Client* client, const std::vector<std::string>& args) {
    if (!client->getIsAuthenticated()) {
        respond(client->getFD(), AERR_PASSWDMISMATCH(client->getNick()));
        client->del();
        return;
    }

    if (args.size() < 4) {
        respond(client->getFD(), AERR_NEEDMOREPARAMS(client->getNick(), "USER"));
        return;
    }

    // concatinate all args after the third one
    std::string realName = args[3];
    for (size_t i = 4; i < args.size(); i++) {
        realName += " " + args[i];
    }

    if (!client->isValidUsername(args[0])) {
        respond(client->getFD(), AERR_ERRONEUSNICKNAME(client->getNick(), realName));
        return;
    }

    client->setUsername(args[0]);
    client->setHostname(args[2]);
}

void CommandHandler::handleRegistration(Client* client, const std::string& command,
                                        const std::vector<std::string>& args) {
    if (command == "QUIT") {
        client->del();
    } else if (command == "CAP" && args[0] == "LS") {
        respond(client->getFD(), ARPL_CAP_LS());
    } else if (command == "PASS") {
        handleRegistrationPass(client, args, this->config->getPassword());
    } else if (command == "NICK") {
        handleRegistrationNick(client, args, this);
    } else if (command == "USER") {
        handleRegistrationUser(client, args);
    }

    if (client->canBeRegistered()) client->registerClient();
}
