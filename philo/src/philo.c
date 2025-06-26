/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 09:29:58 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/26 20:21:26 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>
#include <stdio.h>

#include "common.h"

// Initialize one philosopher. Sets up die/eat/sleep timings and assigns forks.
// For the last philosopher at the table, the order of the forks is swapped to
// avoid the deadlock where all philosophers grab the fork to their right. For
// even-numbered philosophers, a small pre-delay is added at the start of the
// simulation to put all philosophers in a synchronized alternating pattern.
// This pre-delay is not added in the single-philosopher case.

void	philo_init(t_philo *p, t_monitor *m, int idx, int arguments[5])
{
	p->index = idx + 1;
	p->time_to_die = 1000L * arguments[1];
	p->time_to_eat = 1000L * arguments[2];
	p->time_to_sleep = 1000L * arguments[3];
	p->meal_limit = arguments[4];
	p->fork1 = &m->mutexes[idx * (idx != m->thread_count - 1)];
	p->fork2 = &m->mutexes[idx + (idx != m->thread_count - 1)];
	p->print_mutex = &m->print_mutex;
	p->predelay = (idx % 2 == 0) * (m->thread_count != 1) * p->time_to_eat / 2;
}

// Special handling for the single-philosopher case. If there's only one
// philosopher, then there's also only one fork, so the fork pointers refer to
// the same mutex. If the philosopher were to try and take two forks, a deadlock
// would occur. So instead we just wait for the philosopher to starve, since
// they can't start eating without a second fork.

static void	*handle_single_philo(t_philo *p)
{
	wait_for(p->time_to_die);
	pthread_mutex_unlock(p->fork1);
	philo_print(p, STATE_DIED);
	p->stop = true;
	return (NULL);
}

// Wait for the simulation to start. The monitor thread sets a starting time
// when all threads have been created. If one or more threads could not be
// created, this loop exits and the program shuts down.

static void	wait_for_start(t_philo *p)
{
	while (!p->stop && !p->start_time)
		wait_for(START_DELAY / 10);
	wait_until(p->start_time);
}

// Main philosopher loop. Waits for a pre-determined start time, and then cycles
// through the steps of thinking, taking forks, eating, and sleeping. A small
// delay is added at the start of the thinking step, so that if there's another
// philosopher waiting for that fork, that philosopher is allowed to go first
// (since it's likely "hungrier" than one that just finished eating/sleeping).

void	*philo_main(void *arg)
{
	t_philo *const	p = (t_philo*) arg;

	wait_for_start(p);
	while (!p->stop)
	{
		philo_print(p, STATE_THINKING);
		wait_for(p->predelay * (p->meal_count == 0) + THINK_DELAY);
		pthread_mutex_lock(p->fork1);
		philo_print(p, STATE_TAKEN_A_FORK);
		if (p->fork1 == p->fork2)
			return (handle_single_philo(p));
		pthread_mutex_lock(p->fork2);
		philo_print(p, STATE_TAKEN_A_FORK);
		philo_print(p, STATE_EATING);
		p->meal_time = current_time();
		wait_for(p->time_to_eat);
		pthread_mutex_unlock(p->fork1);
		pthread_mutex_unlock(p->fork2);
		if (++p->meal_count == p->meal_limit)
			break ;
		philo_print(p, STATE_SLEEPING);
		wait_for(p->time_to_sleep);
	}
	return (NULL);
}

// Print the current state of a philosopher. A lock is held while printing, to
// avoid messages getting mixed up. When a death message has been printed, no
// more messages are produced.

void	philo_print(t_philo *p, t_state state)
{
	int					timestamp;
	static bool			death_printed;
	static char *const	state_names[] = {
		"is sleeping",
		"is thinking",
		"has taken a fork",
		"is eating",
		"died",
	};

	pthread_mutex_lock(p->print_mutex);
	timestamp = (current_time() - p->start_time) / 1000;
	if (!death_printed)
		printf("%d %d %s\n", timestamp, p->index, state_names[state]);
	death_printed = death_printed || state == STATE_DIED;
	pthread_mutex_unlock(p->print_mutex);
}
