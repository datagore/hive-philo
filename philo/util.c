/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   util.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 08:18:21 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/25 13:12:46 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "philo.h"

int64_t	current_time(void)
{
	struct timeval	timeval;

	gettimeofday(&timeval, NULL);
	return (timeval.tv_sec * 1000000 + timeval.tv_usec);
}

void	*malloc_zeroed(size_t size)
{
	void *const	data = malloc(size);

	if (data != NULL)
		memset(data, 0, size);
	return (data);
}

void	print_state(t_philo *philo, int timestamp, t_state state, int index)
{
	static char *const	state_names[] = {
		"is sleeping",
		"is thinking",
		"has taken a fork",
		"has taken a fork",
		"is eating",
		"died",
	};

	if (!philo->stop)
		printf("%d %d %s\n", timestamp, index + 1, state_names[state]);
	if (state == STATE_DIED)
		philo->stop = true;
}

static int	parse_number(const char *str)
{
	int64_t	value;

	value = 0;
	while (*str == ' ' || ('\t' <= *str && *str <= '\r'))
		str++;
	while ('0' <= *str && *str <= '9')
	{
		value = value * 10 + *str++ - '0';
		if (value > INT_MAX)
			return (-1);
	}
	while (*str == ' ' || ('\t' <= *str && *str <= '\r'))
		str++;
	if (*str != '\0' || value == 0)
		return (-1);
	return (value);
}

void	parse_arguments(int argc, char **argv, int arguments[5])
{
	int	i;

	if (argc < 5 || argc > 6)
	{
		printf("usage: ./philo <count> <die> <eat> <sleep> [meals]\n");
		exit(EXIT_FAILURE);
	}
	i = 0;
	arguments[4] = INT_MAX;
	while (i < argc - 1)
	{
		arguments[i] = parse_number(argv[i + 1]);
		if (arguments[i] <= 0)
		{
			printf("error: invalid argument '%s'\n", argv[i + 1]);
			exit(EXIT_FAILURE);
		}
		i++;
	}
}
