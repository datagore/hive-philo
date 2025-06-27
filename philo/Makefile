SRC := \
	src/main.c \
	src/monitor.c \
	src/philo.c \
	src/timing.c \

NAME := philo
HEADERS := src/philo.h
CFLAGS := -Wall -Wextra -Werror
LDFLAGS := -pthread
OBJ := $(SRC:%.c=%.o)

all: $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

$(NAME): $(OBJ)
	cc $^ -o $@ $(LDFLAGS)

%.o: %.c $(HEADERS)
	cc -c $< -o $@ $(CFLAGS)

.PHONY: all clean fclean re
.SECONDARY: $(OBJ)
