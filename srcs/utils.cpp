#include "utils.hpp"

int setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
		return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
