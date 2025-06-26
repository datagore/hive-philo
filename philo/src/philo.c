/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 09:29:58 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/26 11:02:34 by abostrom         ###   ########.fr       */
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

void	diner_init(t_diner *d, t_philo *p, int index, int arguments[5])
{
	d->index = index + 1;
	d->time_to_die = 1000L * arguments[1];
	d->time_to_eat = 1000L * arguments[2];
	d->time_to_sleep = 1000L * arguments[3];
	d->meal_limit = arguments[4];
	d->meal_time = p->start_time;
	d->fork1 = &p->mutexes[index * (index != p->count - 1)];
	d->fork2 = &p->mutexes[index + (index != p->count - 1)];
	d->print_mutex = &p->print_mutex;
	d->predelay = (index % 2 == 0) * (p->count != 1) * d->time_to_eat / 2;
	d->start_time = p->start_time;
}

// Special handling for the single-philosopher case. If there's only one
// philosopher, then there's also only one fork, so the fork pointers refer to
// the same mutex. If the philosopher were to try and take two forks, a deadlock
// would occur. So instead we just wait for the philosopher to starve, since
// they can't start eating without a second fork.

static void	*handle_single_philo(t_diner *diner)
{
	wait_for(diner->time_to_die);
	pthread_mutex_unlock(diner->fork1);
	diner_print(diner, STATE_DIED);
	diner->stop = true;
	return (NULL);
}

// Main philosopher loop. Waits for a pre-determined start time, and then cycles
// through the steps of thinking, taking forks, eating, and sleeping. A small
// delay is added at the start of the thinking step, so that if there's another
// philosopher waiting for that fork, that philosopher is allowed to go first
// (since it's likely "hungrier" than one that just finished eating/sleeping).

void	*diner_main(void *arg)
{
	t_diner *const	diner = (t_diner*) arg;

	wait_until(diner->start_time);
	while (!diner->stop)
	{
		diner_print(diner, STATE_THINKING);
		wait_for(diner->predelay * (diner->meal_count == 0) + 250);
		pthread_mutex_lock(diner->fork1);
		diner_print(diner, STATE_TAKEN_A_FORK);
		if (diner->fork1 == diner->fork2)
			handle_single_philo(diner);
		pthread_mutex_lock(diner->fork2);
		diner_print(diner, STATE_TAKEN_A_FORK);
		diner_print(diner, STATE_EATING);
		diner->meal_time = current_time();
		wait_for(diner->time_to_eat);
		pthread_mutex_unlock(diner->fork1);
		pthread_mutex_unlock(diner->fork2);
		if (++diner->meal_count == diner->meal_limit)
			break ;
		diner_print(diner, STATE_SLEEPING);
		wait_for(diner->time_to_sleep);
	}
	return (NULL);
}

// Print the current state of a philosopher. A lock is held while printing, to
// avoid messages getting mixed up. When a death message has been printed, no
// more messages are produced.

void	diner_print(t_diner *diner, t_state state)
{
	int						timestamp;
	static bool				death_printed;
	static char *const		state_names[] = {
		"is sleeping",
		"is thinking",
		"has taken a fork",
		"is eating",
		"died",
	};

	pthread_mutex_lock(diner->print_mutex);
	timestamp = (current_time() - diner->start_time) / 1000;
	if (!death_printed)
		printf("%d %d %s\n", timestamp, diner->index, state_names[state]);
	death_printed = death_printed || state == STATE_DIED;
	pthread_mutex_unlock(diner->print_mutex);
}
