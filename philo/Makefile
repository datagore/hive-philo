SRC := \
	src/main.c \
	src/monitor.c \
	src/philo.c \
	src/timing.c \

NAME := philo
HEADERS := src/common.h
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
