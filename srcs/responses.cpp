#include "response.hpp"



static void respondWithQueue(int fd, std::string msg, std::queue<message> *passedQueue) {
    static std::queue<message>* messageQueue;
    if (passedQueue != NULL) {
        messageQueue = passedQueue;
        return;
    } 
    messageQueue->push(std::make_pair(fd, msg));
}

void respondSetQueue(std::queue<message>* messageQueue) {
    respondWithQueue(0, "", messageQueue);
}

void respond(int fd, std::string msg) {
    respondWithQueue(fd, msg, NULL);
}

void respondYouHaveNotRegistered(int fd, std::queue<message>* messageQueue) {
    std::string response = ResponseBuilder("ircserv"
        ).addCommand(ERR_NOTREGISTERED
        ).addParameters(""
        ).addTrailing("You have not registered"
        ).build();
    messageQueue->push(std::make_pair(fd, response));
}
