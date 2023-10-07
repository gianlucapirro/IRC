#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include <queue>
#include <string>

typedef std::pair<int, std::string> message;

// /* Error Responses */

#define AERR_NOTREGISTERED(client)                       ":ircserv 451 " + client + " :You have not registered\r\n"
#define AERR_ALREADYREGISTERED(source)                   ":ircserv 462 " + client + " :You may not register\r\n"
#define AERR_PASSWDMISMATCH(client)                      ":ircserv 464 " + client + " :Password is incorrect\r\n"
#define AERR_NONICKNAMEGIVEN(client)                     ":ircserv 431 " + client + " :Nickname not given\r\n"
#define AERR_NICKNAMEINUSE(client, nick)                 ":ircserv 433 " + client + " " + nick  + " :Nickname is already in use\r\n"
#define AERR_ERRONEUSNICKNAME(client, nickname)          ":ircserv 432 " + client + " " + nickname + " :Erroneous nickname\r\n"
#define AERR_UNKNOWNCOMMAND(client, command)             ":ircserv 421 " + client + " " + command + " :Unknown command\r\n"
#define AERR_NEEDMOREPARAMS(client, command)             ":ircserv 461 " + client + " " + command + " :Not enough parameters\r\n"
#define AERR_TOOMANYCHANNELS(client, channel)            ":ircserv 405 " + client + " " + channel + " :You have joined too many channels\r\n"
#define AERR_NOTONCHANNEL(client, channel)               ":ircserv 442 " + client + " " + channel + " :You're not on that channel\r\n"
#define AERR_NOSUCHCHANNEL(client, channel)              ":ircserv 403 " + client + " " + channel + " :No such channel\r\n"
#define AERR_BADCHANNELKEY(client, channel)              ":ircserv 475 " + client + " " + channel + " :Cannot join channel (+k)\r\n"
#define AERR_CHANNELISFULL(client, channel)              ":ircserv 471 " + client + " " + channel + " :Cannot join channel (+l)\r\n"
#define AERR_CANNOTSENDTOCHAN(client, channel)           ":ircserv 404 " + client + " " + channel + " :Cannot send to channel\r\n"
#define AERR_CHANOPRIVSNEEDED(client, channel)           ":ircserv 482 " + client + " " + channel + " :You're not channel operator\r\n"
#define AERR_NOSUCHNICK(client, nickname)                ":ircserv 401 " + client + " " + nickname + " :No such nick/channel\r\n"
#define AERR_USERNOTINCHANNEL(client, nickname, channel) ":ircserv 441 " + client + " " + nickname + " " + channel + " :They aren't on that channel\r\n"
#define AERR_USERONCHANNEL(client, nickname, channel)    ":ircserv 443 " + client + " " + nickname + " " + channel + " :is already on channel\r\n"
#define AERR_UNKNOWNMODE(client, mode)                   ":ircserv 472 " + client + " " + mode + " :is unknown mode char to me\r\n"


#define AERR_INVITEONLYCHAN(client, channel)             ":ircserv 473 " + client + " " + channel + " :You are not invited to this channel\r\n"


/* Replies */

#define ARPL_WELCOME(client)                             "001 " + client + " :Welcome " + client + " to the Internet Relay Network"
#define ARPL_NAMREPLY(client, channel, users)            ":ircserv 353 " + client + " = " + channel + " :" + users
#define ARPL_ENDOFNAMES(client, channel)                 ":ircserv 366 " + client + " " + channel + " :End of /NAMES list."
#define ARPL_NICK(client, nick)                          ":" + client + " NICK :" + nick
#define ARPL_JOIN(client, channel)                       ":" + client + " JOIN :" + channel
#define ARPL_PART(client, channel)                       ":" + client + " PART :" + channel
#define ARPL_PING(client, token)                         ":" + client + " PONG :" + token
#define ARPL_PRIVMSG(client, target, msg)                ":" + client + " PRIVMSG " + target + " :" + msg 
#define ARPL_NOTICE(client, target, msg)                 ":" + client + " NOTICE " + target + " :" + msg 
#define ARPL_QUIT(client, msg)                           ":" + client + " QUIT :Quit: " + msg 
#define ARPL_INVITE(client, channel, invitee)            ":" + client + " INVITE " + invitee + " :" + channel
#define ARPL_KICK(client, channel, target, reason)       ":" + client + " KICK " + channel + " " + target + " :" + reason
#define ARPL_MODE(client, channel, modes, args)          ":" + client + " MODE " + channel + " " + modes + " " + args
#define ARPL_CAP_LS()                                    ":ircserv CAP * LS :" // Add the capabilities
#define ARPL_NOTOPIC(client, channel)                    ":ircserv 331 " + client + " " + channel + " : No topic is set"
#define ARPL_TOPIC(client, channel, topic)               ":ircserv 332 " + client + " " + channel + " :" + topic
#define ARPL_INVITING(client, channel, invitee)          ":ircserv 341 " + client + " " + channel + " " + invitee + " :"


typedef std::pair<int, std::string> message;

void respondSetQueue(std::queue<message>* messageQueue);
void respond(int fd, std::string msg);

#endif