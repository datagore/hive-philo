/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 09:29:37 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/26 11:12:24 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "common.h"

// Set up the simulation. Initializes all mutexes and threads. If a thread
// couldn't be created, the simulation ends immediately. All threads are told to
// delay until an arbitrary starting time, to synchronize their actions.

void	philo_begin(t_philo *p, int arguments[5])
{
	t_diner	*diner;
	int		i;

	i = 0;
	p->count = arguments[0];
	while (i < p->count)
		pthread_mutex_init(&p->mutexes[i++], NULL);
	pthread_mutex_init(&p->print_mutex, NULL);
	i = 0;
	p->start_time = current_time() + START_DELAY;
	while (i < p->count)
	{
		diner = &p->diners[i];
		diner_init(diner, p, i, arguments);
		if (pthread_create(&diner->thread, NULL, diner_main, diner))
		{
			printf("error: pthread_create failed\n");
			break ;
		}
		p->started++;
		i++;
	}
}

// Main monitoring loop. Continually monitors the philosophers, stopping when
// all meals have been finished, or when a philosopher dies of starvation.

void	philo_loop(t_philo *p)
{
	int		i;
	int		finished_count;
	int64_t	now;

	finished_count = 0;
	while (p->started == p->count && finished_count < p->count)
	{
		i = 0;
		finished_count = 0;
		now = current_time();
		while (i < p->count)
		{
			if (p->diners[i].stop)
				return ;
			if (p->diners[i].meal_count == p->diners[i].meal_limit)
				finished_count++;
			else if (p->diners[i].meal_time + p->diners[i].time_to_die <= now)
				return (diner_print(&p->diners[i], STATE_DIED));
			i++;
		}
		usleep(MONITOR_RATE);
	}
}

// End the simulation, stopping all philosopher threads that were successfully
// created, waiting for them to finish, and then disposing of all mutexes.

void	philo_end(t_philo *p)
{
	int	i;

	i = 0;
	while (i < p->started)
		p->diners[i++].stop = true;
	i = 0;
	while (i < p->started)
		pthread_join(p->diners[i++].thread, NULL);
	i = 0;
	while (i < p->count)
		pthread_mutex_destroy(&p->mutexes[i++]);
	pthread_mutex_destroy(&p->print_mutex);
}
