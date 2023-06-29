#include "ResponseBuilder.hpp"

ResponseBuilder::ResponseBuilder(const std::string& prefix) : prefix(prefix) {}

ResponseBuilder& ResponseBuilder::addPrefix(const std::string& prefix) {
    this->prefix = prefix;
    return *this;
}

ResponseBuilder& ResponseBuilder::addCommand(const std::string& command) {
    this->responseStream << " " << command;
    return *this;
}

ResponseBuilder& ResponseBuilder::addParameters(const std::string& parameters) {
    this->responseStream << " " << parameters;
    return *this;
}

ResponseBuilder& ResponseBuilder::addTrailing(const std::string& trailing) {
    this->responseStream << " :" << trailing;
    return *this;
}

std::string ResponseBuilder::build() { return ":" + this->prefix + this->responseStream.str() + "\r\n"; }