NAME = ircserv
CFLAGS = -Wall -Werror -Wextra -std=c++98 -fsanitize=address
LDFLAGS =
CC = c++
VPATH = src/
SRC = 	main.cpp \
		Channel.cpp \
		Command.cpp \
		Log.cpp \
		Server.cpp \
		User.cpp	\
		Utils.cpp

OBJDIR = obj/

OBJ = $(addprefix $(OBJDIR), $(patsubst %.cpp, %.o, $(SRC)))

all: $(NAME)

$(OBJDIR)%.o: %.cpp
	$(CC) $(CFLAGS) -c  $< -o $@

$(OBJDIR):
	mkdir $(OBJDIR)

$(NAME): $(OBJDIR) $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all