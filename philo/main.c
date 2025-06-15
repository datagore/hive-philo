/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 23:32:05 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/15 23:05:11 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "philo.h"

static long	get_time(void)
{
	struct timeval	timeval;

	gettimeofday(&timeval, NULL);
	return (timeval.tv_sec * 1000000 + timeval.tv_usec);
}

static void	sleep_for(long usecs)
{
	const long	start_time = get_time();
	const long	target_time = start_time + usecs;
	long		difference;

	difference = usecs;
	while (difference > 0)
	{
		usleep(difference);
		difference = target_time - get_time();
	}
}

static int	parse_argument(const char *str)
{
	int	value;

	value = 0;
	while (*str == ' ' || ('\t' <= *str && *str <= '\r'))
		str++;
	while ('0' <= *str && *str <= '9')
		value = value * 10 + *str++ - '0';
	while (*str == ' ' || ('\t' <= *str && *str <= '\r'))
		str++;
	if (*str != '\0' || value <= 0)
	{
		printf("error: invalid argument\n");
		exit(EXIT_FAILURE);
	}
	return (value);
}

static void	print_event(t_philo *philo, t_event_type type, int index)
{
	static const char *const	event_string[] = {
		"took a fork",
		"is eating",
		"is sleeping",
		"is thinking",
		"died",
	};
	long						timestamp;

	timestamp = (get_time() - philo->start_time + 500) / 1000;
	if (pthread_mutex_lock(&philo->print_mutex) == 0)
	{
		printf("%ld %d %s\n", timestamp, index, event_string[type]);
		pthread_mutex_unlock(&philo->print_mutex);
	}
}

static void	*philo_thread(void *arg)
{
	t_philo *const	philo = (t_philo*) arg;
	const int		index = philo->created++;
	const int		left_fork = (index != philo->count - 1) * index;
	const int		right_fork = (index != philo->count - 1) + index;
	int				meals_eaten;

	meals_eaten = 0;
	while (meals_eaten++ < philo->max_meals)
	{
		pthread_mutex_lock(&philo->mutexes[left_fork]);
		print_event(philo, EVENT_TOOK_FORK, index);
		pthread_mutex_lock(&philo->mutexes[right_fork]);
		print_event(philo, EVENT_EATING, index);
		sleep_for(philo->eat_time);
		print_event(philo, EVENT_SLEEPING, index);
		pthread_mutex_unlock(&philo->mutexes[left_fork]);
		pthread_mutex_unlock(&philo->mutexes[right_fork]);
		sleep_for(philo->sleep_time);
		print_event(philo, EVENT_THINKING, index);
	}
	return (NULL);
}

static void	philo_main(t_philo *philo)
{
	int		i;

	philo->start_time = get_time();
	pthread_mutex_init(&philo->print_mutex, NULL);
	i = 0;
	while (i < philo->count)
		pthread_mutex_init(&philo->mutexes[i++], NULL);
	i = 0;
	while (i < philo->count)
	{
		if (pthread_create(&philo->threads[i++], NULL, philo_thread, philo))
		{
			printf("error: pthread_create failed\n");
			return ;
		}
	}
	i = 0;
	while (i < philo->count)
		pthread_join(philo->threads[i++], NULL);
}

int	main(int argc, char **argv)
{
	t_philo	philo;

	memset(&philo, 0, sizeof(t_philo));
	if (argc != 5 && argc != 6)
		return (printf("usage: %s <count> <die> <eat> [sleep]\n", argv[0]));
	philo.count = parse_argument(argv[1]);
	philo.starve_time = 1000 * parse_argument(argv[2]);
	philo.eat_time = 1000 * parse_argument(argv[3]);
	philo.sleep_time = 1000 * parse_argument(argv[4]);
	philo.max_meals = INT_MAX;
	if (argc == 6)
		philo.max_meals = parse_argument(argv[5]);
	philo.threads = malloc(philo.count * sizeof(pthread_t *));
	philo.mutexes = malloc(philo.count * sizeof(pthread_mutex_t));
	if (philo.threads != NULL && philo.mutexes != NULL)
		philo_main(&philo);
	else
		printf("error: can't allocate memory\n");
	free(philo.threads);
	free(philo.mutexes);
}
