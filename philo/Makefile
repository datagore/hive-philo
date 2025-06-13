SRC := main.c
OBJ := $(SRC:src/%.c=src/%.o)
NAME := philo
CFLAGS := -Wall -Wextra -Werror
LDFLAGS := -pthread

all: $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

$(NAME): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

.PHONY: all clean fclean re
.SECONDARY: $(OBJ)
