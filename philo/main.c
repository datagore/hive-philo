/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 23:32:05 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/24 09:09:27 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "philo.h"

static void	*diner_thread(void *arg)
{
	t_diner *const	diner = (t_diner*) arg;

	while (!diner->ended && diner->meal_count < diner->meal_limit)
	{
		diner->state++;
		pthread_mutex_lock(diner->fork1);
		diner->state++;
		if (diner->fork1 == diner->fork2)
		{
			usleep(diner->time_to_die);
			pthread_mutex_unlock(diner->fork1);
			break ;
		}
		pthread_mutex_lock(diner->fork2);
		diner->state += 2;
		diner->meal_time = current_time();
		usleep(diner->time_to_eat);
		diner->meal_count++;
		pthread_mutex_unlock(diner->fork1);
		pthread_mutex_unlock(diner->fork2);
		diner->state++;
		usleep(diner->time_to_sleep);
		usleep(750);
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
		p->diners[i].meal_time = p->start_time;
		p->diners[i].fork1 = &p->mutexes[i * (i != p->count - 1)];
		p->diners[i].fork2 = &p->mutexes[i + (i != p->count - 1)];
		if (pthread_create(&p->threads[i], NULL, diner_thread, &p->diners[i]))
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
			else if (p->diners[i].time_to_die + p->diners[i].meal_time <= now)
				print_state(p, STATE_DIED, i);
			while (p->states[i] < state)
				print_state(p, ++p->states[i] % STATE_MAX, i);
			i++;
		}
		usleep(500);
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
