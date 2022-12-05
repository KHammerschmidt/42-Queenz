NAME = ircserv
CFLAGS = -Wall -Werror -Wextra -std=c++98
CC = c++
SRC = 	main.cpp	\
		./src/*.cpp
TEST
OBJ = $(SRC.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(SRC) -o $@

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all