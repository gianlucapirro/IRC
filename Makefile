# Compiler
CC = c++

# Compiler Flags
CFLAGS = -Wall -Wextra -Werror -std=c++98

# Includes
INCLUDES = -Iincludes

# Name of your output program
PROG = ircserv

# Source files
SRC = Main.cpp \
      Server.cpp \
      ServerIO.cpp \
      ServerSetup.cpp \
	  ServerHelpers.cpp \
      ServerAccessors.cpp \
      CommandHandler.cpp \
	  CommandHandlerRegistration.cpp \
	  responses.cpp \
      ChannelHandler.cpp \
      ChannelHandlerJoin.cpp \
	  ChannelHandlerLeave.cpp \
	  invite.cpp \
      Mode.cpp \
      Channel.cpp \
      Client.cpp \
      Config.cpp \
	  utils.cpp

SRC := $(addprefix srcs/, $(SRC))

OBJS = $(patsubst srcs/%.cpp, objs/%.o, $(SRC))

vpath %.cpp $(shell find srcs -type d)

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

objs/%.o: %.cpp
	@mkdir -p objs
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf objs

fclean: clean
	rm -f $(PROG)

re: fclean all
