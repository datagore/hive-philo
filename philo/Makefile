NAME := philo
SRC := main.c util.c
HEADERS := philo.h
CFLAGS := -Wall -Wextra -Werror
LDFLAGS := -pthread
OBJ := $(SRC:%.c=%.o)

all: $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

$(NAME): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) -c $< -o $@ $(CFLAGS)

.PHONY: all clean fclean re
.SECONDARY: $(OBJ)
