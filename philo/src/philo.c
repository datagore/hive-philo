/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 09:29:58 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/26 19:51:04 by abostrom         ###   ########.fr       */
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

void	philo_init(t_philo *philo, t_monitor *m, int idx, int arguments[5])
{
	philo->index = idx + 1;
	philo->time_to_die = 1000L * arguments[1];
	philo->time_to_eat = 1000L * arguments[2];
	philo->time_to_sleep = 1000L * arguments[3];
	philo->meal_limit = arguments[4];
	philo->meal_time = m->start_time;
	philo->fork1 = &m->mutexes[idx * (idx != m->count - 1)];
	philo->fork2 = &m->mutexes[idx + (idx != m->count - 1)];
	philo->print_mutex = &m->print_mutex;
	philo->predelay = (idx % 2 == 0) * (m->count != 1) * philo->time_to_eat / 2;
	philo->start_time = m->start_time;
}

// Special handling for the single-philosopher case. If there's only one
// philosopher, then there's also only one fork, so the fork pointers refer to
// the same mutex. If the philosopher were to try and take two forks, a deadlock
// would occur. So instead we just wait for the philosopher to starve, since
// they can't start eating without a second fork.

static void	*handle_single_philo(t_philo *philo)
{
	wait_for(philo->time_to_die);
	pthread_mutex_unlock(philo->fork1);
	philo_print(philo, STATE_DIED);
	philo->stop = true;
	return (NULL);
}

// Main philosopher loop. Waits for a pre-determined start time, and then cycles
// through the steps of thinking, taking forks, eating, and sleeping. A small
// delay is added at the start of the thinking step, so that if there's another
// philosopher waiting for that fork, that philosopher is allowed to go first
// (since it's likely "hungrier" than one that just finished eating/sleeping).

void	*philo_main(void *arg)
{
	t_philo *const	philo = (t_philo*) arg;

	wait_until(philo->start_time);
	while (!philo->stop)
	{
		philo_print(philo, STATE_THINKING);
		wait_for(philo->predelay * (philo->meal_count == 0) + THINK_DELAY);
		pthread_mutex_lock(philo->fork1);
		philo_print(philo, STATE_TAKEN_A_FORK);
		if (philo->fork1 == philo->fork2)
			return (handle_single_philo(philo));
		pthread_mutex_lock(philo->fork2);
		philo_print(philo, STATE_TAKEN_A_FORK);
		philo_print(philo, STATE_EATING);
		philo->meal_time = current_time();
		wait_for(philo->time_to_eat);
		pthread_mutex_unlock(philo->fork1);
		pthread_mutex_unlock(philo->fork2);
		if (++philo->meal_count == philo->meal_limit)
			break ;
		philo_print(philo, STATE_SLEEPING);
		wait_for(philo->time_to_sleep);
	}
	return (NULL);
}

// Print the current state of a philosopher. A lock is held while printing, to
// avoid messages getting mixed up. When a death message has been printed, no
// more messages are produced.

void	philo_print(t_philo *philo, t_state state)
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

	pthread_mutex_lock(philo->print_mutex);
	timestamp = (current_time() - philo->start_time) / 1000;
	if (!death_printed)
		printf("%d %d %s\n", timestamp, philo->index, state_names[state]);
	death_printed = death_printed || state == STATE_DIED;
	pthread_mutex_unlock(philo->print_mutex);
}
