/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 23:32:05 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/24 00:38:53 by abostrom         ###   ########.fr       */
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

static int64_t	current_time(void)
{
	struct timeval	timeval;

	gettimeofday(&timeval, NULL);
	return (timeval.tv_sec * 1000000 + timeval.tv_usec);
}

static int	read_number(const char *str)
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

static void	print_state(t_philo *philo, t_state state, int index)
{
	int					timestamp;
	static char *const	state_names[] = {
		"is sleeping",
		"is thinking",
		"took a fork",
		"took a fork",
		"is eating",
		"died",
	};

	timestamp = (current_time() - philo->start_time) / 1000;
	if (!philo->ended)
		printf("%d %d %s\n", timestamp, index + 1, state_names[state]);
	if (state == STATE_DIED)
		philo->ended = true;
}

static void	*diner_main(void *arg)
{
	t_diner *const	d = (t_diner*) arg;

	while (!d->ended && d->meal_count < d->meal_limit)
	{
		d->state++;
		pthread_mutex_lock(d->fork1);
		d->state++;
		if (d->fork1 == d->fork2)
		{
			usleep(d->time_to_die);
			pthread_mutex_unlock(d->fork1);
			break ;
		}
		pthread_mutex_lock(d->fork2);
		d->state += 2;
		usleep(d->time_to_eat);
		d->meal_time = current_time();
		d->meal_count++;
		pthread_mutex_unlock(d->fork1);
		pthread_mutex_unlock(d->fork2);
		d->state++;
		usleep(d->time_to_sleep);
		usleep(500);
	}
	return (NULL);
}

static void	philo_begin(t_philo *p, int arguments[5])
{
	int	i;

	i = 0;
	p->count = arguments[0];
	while (i < p->count)
		pthread_mutex_init(&p->mutexes[i++], NULL);
	p->start_time = current_time();
	i = 0;
	while (i < p->count)
	{
		p->diners[i].time_to_die = 1000LL * arguments[1];
		p->diners[i].time_to_eat = 1000LL * arguments[2];
		p->diners[i].time_to_sleep = 1000LL * arguments[3];
		p->diners[i].meal_limit = arguments[4];
		p->diners[i].fork1 = &p->mutexes[i * (i != p->count - 1)];
		p->diners[i].fork2 = &p->mutexes[i + (i != p->count - 1)];
		p->diners[i].meal_time = p->start_time;
		if (pthread_create(&p->threads[i], NULL, diner_main, &p->diners[i]))
		{
			printf("error: pthread_create failed\n");
			break ;
		}
		p->started++;
		i++;
	}
}

static void	philo_loop(t_philo *p)
{
	int		i;
	int		finished;
	int64_t	now;
	int64_t	state;

	finished = 0;
	while (p->started == p->count && !p->ended && finished < p->count)
	{
		i = 0;
		finished = 0;
		now = current_time();
		while (i < p->count)
		{
			state = p->diners[i].state;
			if (p->diners[i].meal_count == p->diners[i].meal_limit)
				finished++;
			else if (p->diners[i].meal_time + p->diners[i].time_to_die < now)
				print_state(p, STATE_DIED, i);
			while (p->states[i] < state)
				print_state(p, ++p->states[i] % STATE_MAX, i);
			i++;
		}
		usleep(100);
	}
}

static void	philo_end(t_philo *p)
{
	int	i;

	i = 0;
	while (i < p->started)
	{
		p->diners[i].ended = true;
		pthread_join(p->threads[i++], NULL);
	}
	i = 0;
	while (i < p->count)
		pthread_mutex_destroy(&p->mutexes[i++]);
}

static void	parse_arguments(int argc, char **argv, int arguments[5])
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
		arguments[i] = read_number(argv[i + 1]);
		if (arguments[i] <= 0)
		{
			printf("error: invalid argument '%s'\n", argv[i + 1]);
			exit(EXIT_FAILURE);
		}
		i++;
	}
}

static void	*malloc_zeroed(size_t size)
{
	void *const	data = malloc(size);

	if (data != NULL)
		memset(data, 0, size);
	return (data);
}

int	main(int argc, char **argv)
{
	t_philo	philo;
	int		arguments[5];

	parse_arguments(argc, argv, arguments);
	memset(&philo, 0, sizeof(philo));
	philo.count = arguments[0];
	philo.mutexes = malloc_zeroed(philo.count * sizeof(pthread_mutex_t));
	philo.threads = malloc_zeroed(philo.count * sizeof(pthread_t));
	philo.diners = malloc_zeroed(philo.count * sizeof(t_diner));
	philo.states = malloc_zeroed(philo.count * sizeof(int64_t));
	if (!philo.mutexes || !philo.threads || !philo.diners || !philo.states)
		printf("error: can't allocate memory\n");
	else
	{
		philo_begin(&philo, arguments);
		philo_loop(&philo);
		philo_end(&philo);
	}
	free(philo.mutexes);
	free(philo.threads);
	free(philo.diners);
	free(philo.states);
}
