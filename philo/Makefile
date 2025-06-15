SRC := main.c
OBJ := $(SRC:%.c=%.o)
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

%.o: %.c philo.h
	$(CC) -c $< -o $@ $(CFLAGS)

.PHONY: test
test: all
	./$(NAME) 5 500 100 100

.PHONY: all clean fclean re
.SECONDARY: $(OBJ)
