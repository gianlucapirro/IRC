#ifndef RESPONSEBUILDER_HPP
#define RESPONSEBUILDER_HPP

#include <sstream>
#include <string>

class ResponseBuilder {
   private:
    std::string prefix;
    std::stringstream responseStream;

   public:
    ResponseBuilder(const std::string& prefix);

    ResponseBuilder& addPrefix(const std::string& prefix);
    ResponseBuilder& addCommand(const std::string& command);
    ResponseBuilder& addParameters(const std::string& parameters);
    ResponseBuilder& addTrailing(const std::string& trailing);

    std::string build();
};

#endif
