#include "response.hpp"
#include <string>
#include <fcntl.h>


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