/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 23:32:05 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/25 19:22:35 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "philo.h"

static int64_t	start_time;
static bool		stop;

static void	print_state2(t_state state, int index)
{
	static pthread_mutex_t	print_lock;
	int						timestamp;
	static char *const		state_names[] = {
		"is sleeping",
		"is thinking",
		"has taken a fork",
		"has taken a fork",
		"is eating",
		"died",
	};

	pthread_mutex_lock(&print_lock);
	timestamp = (current_time() - start_time) / 1000;
	if (!stop)
		printf("%d %d %s\n", timestamp, index, state_names[state]);
	if (state == STATE_DIED)
		stop = true;
	pthread_mutex_unlock(&print_lock);
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

static void	wait_for(int64_t duration)
{
	wait_until(current_time() + duration);
}

static void	*diner_thread(void *arg)
{
	t_diner *const	diner = (t_diner*) arg;

	wait_until(start_time);
	while (!diner->stop)
	{
		print_state2(++diner->state % STATE_MAX, diner->index);
		wait_for(diner->predelay * (diner->meal_count == 0) + 250);
		pthread_mutex_lock(diner->fork1);
		print_state2(++diner->state % STATE_MAX, diner->index);
		if (diner->fork1 == diner->fork2)
		{
			wait_for(diner->time_to_die);
			print_state2(STATE_DIED, diner->index);
			pthread_mutex_unlock(diner->fork1);
			break ;
		}
		pthread_mutex_lock(diner->fork2);
		print_state2(++diner->state % STATE_MAX, diner->index);
		print_state2(++diner->state % STATE_MAX, diner->index);
		diner->meal_time = current_time();
		wait_for(diner->time_to_eat);
		pthread_mutex_unlock(diner->fork1);
		pthread_mutex_unlock(diner->fork2);
		if (++diner->meal_count == diner->meal_limit)
			break ;
		print_state2(++diner->state % STATE_MAX, diner->index);
		wait_for(diner->time_to_sleep);
	}
	return (NULL);
}

static int	philo_begin(t_philo *p, int arguments[5])
{
	int	i;

	i = 0;
	p->count = arguments[0];
	while (i < p->count)
		pthread_mutex_init(&p->mutexes[i++], NULL);
	i = 0;
	start_time = current_time() + 10000;
	while (i < p->count)
	{
		p->diners[i].time_to_die = 1000LL * arguments[1];
		p->diners[i].time_to_eat = 1000LL * arguments[2];
		p->diners[i].time_to_sleep = 1000LL * arguments[3];
		p->diners[i].meal_limit = arguments[4];
		p->diners[i].meal_time = start_time;
		p->diners[i].fork1 = &p->mutexes[i * (i != p->count - 1)];
		p->diners[i].fork2 = &p->mutexes[i + (i != p->count - 1)];
		p->diners[i].predelay = (i % 2 == 0) * (p->count != 1) * p->diners[i].time_to_eat / 2;
		p->diners[i].index = i + 1;
		if (pthread_create(&p->threads[i], NULL, diner_thread, &p->diners[i]))
			return (printf("error: pthread_create failed\n"));
		p->started++;
		i++;
	}
	return (0);
}

static void	philo_loop(t_philo *p)
{
	int		i;
	int		finished;
	int64_t	now;

	finished = 0;
	wait_until(start_time);
	while (!p->stop && !stop && p->started == p->count && finished < p->count)
	{
		i = 0;
		finished = 0;
		now = current_time();
		while (i < p->count)
		{
			if (p->diners[i].meal_count == p->diners[i].meal_limit)
				finished++;
			else if (p->diners[i].meal_time + p->diners[i].time_to_die <= now)
				print_state2(STATE_DIED, i + 1);
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
		p->diners[i++].stop = true;
	i = 0;
	while (i < p->started)
		pthread_join(p->threads[i++], NULL);
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
