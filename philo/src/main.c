/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 23:32:05 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/27 11:37:57 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "philo.h"

// Convenience function for printing an error message and exiting the program.

static int	error(const char *message)
{
	printf("error: %s\n", message);
	exit(EXIT_FAILURE);
}

// Convert a string to a number. Spaces are stripped from either end of the
// string, but the rest of the string must contain only decimal digits. If the
// string doesn't contain a valid number, or if it overflows INT_MAX, then the
// program exits with an error message.

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
			error("invalid argument");
	}
	while (*str == ' ' || ('\t' <= *str && *str <= '\r'))
		str++;
	if (*str != '\0' || value <= 0)
		error("invalid argument");
	return (value);
}

// Parse command line arguments, storing the numeric value of each in an array.
// If the fifth argument (the maximum number of meals) is left out, then it's
// set to INT_MAX.

static void	parse_arguments(int argc, char **argv, int arguments[5])
{
	if (argc < 5 || argc > 6)
		error("usage: ./philo <count> <die> <eat> <sleep> [meals]");
	arguments[0] = parse_number(argv[1]);
	arguments[1] = parse_number(argv[2]);
	arguments[2] = parse_number(argv[3]);
	arguments[3] = parse_number(argv[4]);
	if (argc == 6)
		arguments[4] = parse_number(argv[5]);
	else
		arguments[4] = INT_MAX;
}

// Program entry point. Parses command line arguments, then allocates memory for
// the required number of threads, and starts the simulation loop. This is the
// only place in the program where memory is allocated or freed.

int	main(int argc, char **argv)
{
	t_monitor	monitor;
	int			arguments[5];

	parse_arguments(argc, argv, arguments);
	memset(&monitor, 0, sizeof(monitor));
	monitor.thread_count = arguments[0];
	monitor.mutexes = malloc(monitor.thread_count * sizeof(pthread_mutex_t));
	monitor.philos = malloc(monitor.thread_count * sizeof(t_philo));
	if (monitor.mutexes == NULL || monitor.philos == NULL)
		printf("error: can't allocate memory\n");
	else
	{
		memset(monitor.philos, 0, monitor.thread_count * sizeof(t_philo));
		monitor_begin(&monitor, arguments);
		monitor_loop(&monitor);
		monitor_end(&monitor);
	}
	free(monitor.mutexes);
	free(monitor.philos);
}
