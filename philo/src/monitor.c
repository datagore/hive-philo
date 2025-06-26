/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 09:29:37 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/26 11:27:00 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "common.h"

// Set up the simulation. Initializes all mutexes and threads. If a thread
// couldn't be created, the simulation ends immediately. All threads are told to
// delay until an arbitrary starting time, to synchronize their actions.

void	monitor_begin(t_monitor *m, int arguments[5])
{
	t_philo	*philo;
	int		i;

	i = 0;
	m->count = arguments[0];
	while (i < m->count)
		pthread_mutex_init(&m->mutexes[i++], NULL);
	pthread_mutex_init(&m->print_mutex, NULL);
	i = 0;
	m->start_time = current_time() + START_DELAY;
	while (i < m->count)
	{
		philo = &m->philos[i];
		philo_init(philo, m, i, arguments);
		if (pthread_create(&philo->thread, NULL, philo_main, philo))
		{
			printf("error: pthread_create failed\n");
			break ;
		}
		m->started++;
		i++;
	}
}

// Main monitoring loop. Continually monitors the philosophers, stopping when
// all meals have been finished, or when a philosopher dies of starvation.

void	monitor_loop(t_monitor *m)
{
	int		i;
	int		finished_count;
	int64_t	now;

	finished_count = 0;
	while (m->started == m->count && finished_count < m->count)
	{
		i = 0;
		finished_count = 0;
		now = current_time();
		while (i < m->count)
		{
			if (m->philos[i].stop)
				return ;
			if (m->philos[i].meal_count == m->philos[i].meal_limit)
				finished_count++;
			else if (m->philos[i].meal_time + m->philos[i].time_to_die <= now)
				return (philo_print(&m->philos[i], STATE_DIED));
			i++;
		}
		usleep(MONITOR_RATE);
	}
}

// End the simulation, stopping all philosopher threads that were successfully
// created, waiting for them to finish, and then disposing of all mutexes.

void	monitor_end(t_monitor *m)
{
	int	i;

	i = 0;
	while (i < m->started)
		m->philos[i++].stop = true;
	i = 0;
	while (i < m->started)
		pthread_join(m->philos[i++].thread, NULL);
	i = 0;
	while (i < m->count)
		pthread_mutex_destroy(&m->mutexes[i++]);
	pthread_mutex_destroy(&m->print_mutex);
}
