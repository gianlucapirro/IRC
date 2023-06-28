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
      ServerAccessors.cpp \
      ServerIO.cpp \
      ServerSetup.cpp \
      Client.cpp \
      CommandHandler.cpp \
      Config.cpp 

# Prefix 'srcs/' to each source file
SRC := $(addprefix srcs/, $(SRC))

# Object files with 'objs/' prefix and .o extension
OBJS = $(patsubst srcs/%.cpp, objs/%.o, $(SRC))

# vpath directive to search for the .cpp files in all subdirectories of srcs/
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

server:
	docker-compose up --build -d
	docker exec -it irc-irc_server-1 bash

valgrind: re
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./ircserv 1234 password

irssi:
	docker exec -it irc-irc_server-1 irssi
