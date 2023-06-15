# Compiler
CC = c++

# Compiler Flags
CFLAGS = -Wall -Wextra -Werror -std=c++98

# Name of your output program
PROG = ircserv

# Source files
SRC =	Main.cpp \
		Server.cpp 

all: $(PROG)

$(PROG): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o

fclean: clean
	rm -f $(PROG)

re: fclean all
