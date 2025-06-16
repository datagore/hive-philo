/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 23:32:05 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/16 10:15:39 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "philo.h"

static int64_t	min(int64_t a, int64_t b)
{
	return (a * (a < b) + b * (a >= b));
}

static int64_t	current_time(void)
{
	struct timeval	timeval;

	gettimeofday(&timeval, NULL);
	return (timeval.tv_sec * 1000000 + timeval.tv_usec);
}

static void	wait_until(int64_t target_time)
{
	int64_t	difference;

	difference = target_time - current_time();
	while (difference > 0)
	{
		usleep(difference);
		difference = target_time - current_time();
	}
}

static int	read_number(const char *str)
{
	int64_t	value;

	value = 0;
	while (*str == ' ' || ('\t' <= *str && *str <= '\r'))
		str++;
	while ('0' <= *str && *str <= '9')
		value = value * 10 + *str++ - '0';
	while (*str == ' ' || ('\t' <= *str && *str <= '\r'))
		str++;
	if (*str != '\0' || value <= 0)
	{
		printf("error: invalid argument '%s'\n", str);
		exit(EXIT_FAILURE);
	}
	return (value);
}

static int	write_string(char *buffer, char *string)
{
	int	length;

	length = 0;
	while (*string != '\0')
		buffer[length++] = *string++;
	buffer[length++] = '\n';
	return (length);
}

static int	write_number(char *buffer, unsigned int number)
{
	unsigned int	length;
	unsigned int	digits;
	unsigned int	temp;

	temp = number;
	digits = number == 0;
	while (temp > 0)
	{
		temp /= 10;
		digits++;
	}
	length = digits;
	while (digits-- > 0)
	{
		buffer[digits] = '0' + number % 10;
		number /= 10;
	}
	buffer[length++] = ' ';
	return (length);
}

static void	print_event(t_philo *philo, t_event_type event, int index)
{
	char		buffer[50];
	char		*end;
	uint64_t	timestamp;

	timestamp = (current_time() - philo->start_time + 500) / 1000;
	end = buffer;
	end += write_number(end, timestamp);
	end += write_number(end, index);
	if (event == EVENT_TOOK_FORK)
		end += write_string(end, "took a fork");
	if (event == EVENT_EATING)
		end += write_string(end, "is eating");
	if (event == EVENT_SLEEPING)
		end += write_string(end, "is sleeping");
	if (event == EVENT_THINKING)
		end += write_string(end, "is thinking");
	if (event == EVENT_DIED)
		end += write_string(end, "died");
	pthread_mutex_lock(&philo->print_mutex);
	if (!philo->ended)
		write(STDOUT_FILENO, buffer, end - buffer);
	if (event == EVENT_DIED)
		philo->ended = true;
	pthread_mutex_unlock(&philo->print_mutex);
}

static bool	check_if_starved(t_philo *philo, int index, int64_t starve_time)
{
	if (philo->ended || current_time() >= starve_time)
	{
		print_event(philo, EVENT_DIED, index);
		return (true);
	}
	return (false);
}

static void	*philo_thread(void *arg)
{
	t_philo *const	philo = (t_philo*) arg;
	const int		index = philo->created++;
	const bool		last = index == philo->count - 1;
	const int		fork_number[2] = {!last * index, !last + index};
	int				meals_eaten;
	int64_t			done_eating;
	int64_t			done_sleeping;
	int64_t			starved;

	meals_eaten = 0;
	starved = philo->start_time + philo->starve_time;
	while (meals_eaten++ < philo->max_meals)
	{
		print_event(philo, EVENT_THINKING, index);
		pthread_mutex_lock(&philo->mutexes[fork_number[0]]);
		if (check_if_starved(philo, index, starved))
		{
			pthread_mutex_unlock(&philo->mutexes[fork_number[0]]);
			return (NULL);
		}
		print_event(philo, EVENT_TOOK_FORK, index);
		if (philo->count == 1)
		{
			wait_until(starved);
			print_event(philo, EVENT_DIED, index);
			pthread_mutex_unlock(&philo->mutexes[fork_number[0]]);
			return (NULL);
		}
		pthread_mutex_lock(&philo->mutexes[fork_number[1]]);
		if (check_if_starved(philo, index, starved))
		{
			pthread_mutex_unlock(&philo->mutexes[fork_number[0]]);
			pthread_mutex_unlock(&philo->mutexes[fork_number[1]]);
			return (NULL);
		}
		print_event(philo, EVENT_EATING, index);
		done_eating = current_time() + philo->eat_time;
		starved = current_time() + philo->starve_time;
		wait_until(min(done_eating, starved));
		pthread_mutex_unlock(&philo->mutexes[fork_number[0]]);
		pthread_mutex_unlock(&philo->mutexes[fork_number[1]]);
		if (check_if_starved(philo, index, starved))
			return (NULL);
		print_event(philo, EVENT_SLEEPING, index);
		done_sleeping = done_eating + philo->sleep_time;
		wait_until(min(done_sleeping, starved));
		if (check_if_starved(philo, index, starved))
			return (NULL);
	}
	return (NULL);
}

static void	philo_main(t_philo *philo)
{
	int		i;

	philo->start_time = current_time();
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
	i = 0;
	while (i < philo->count)
		pthread_mutex_destroy(&philo->mutexes[i++]);
}

int	main(int argc, char **argv)
{
	t_philo	philo;

	memset(&philo, 0, sizeof(t_philo));
	if (argc != 5 && argc != 6)
	{
		printf("usage: %s <count> <die> <eat> <sleep> [meals]\n", argv[0]);
		return (1);
	}
	philo.count = read_number(argv[1]);
	philo.starve_time = 1000 * read_number(argv[2]);
	philo.eat_time = 1000 * read_number(argv[3]);
	philo.sleep_time = 1000 * read_number(argv[4]);
	philo.max_meals = INT_MAX;
	if (argc == 6)
		philo.max_meals = read_number(argv[5]);
	philo.threads = malloc(philo.count * sizeof(pthread_t *));
	philo.mutexes = malloc(philo.count * sizeof(pthread_mutex_t));
	if (philo.threads != NULL && philo.mutexes != NULL)
		philo_main(&philo);
	else
		printf("error: can't allocate memory\n");
	free(philo.threads);
	free(philo.mutexes);
}
