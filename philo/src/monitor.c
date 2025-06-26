/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 09:29:37 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/26 20:23:41 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "common.h"

// If all philosopher threads were successfully created, a starting time for the
// simulation is set, which will eventually set the philosopher threads in
// motion. A small delay is added before the start time, to give all threads a
// chance to get a synchronized start. The timestamp of each philosopher's last
// meal is set to the starting time, to stop them from starving immediately.

static void	start_philo_threads(t_monitor *m)
{
	int64_t	start_time;
	int		i;

	if (m->threads_created < m->thread_count)
		return ;
	i = 0;
	start_time = current_time() + START_DELAY;
	while (i < m->thread_count)
	{
		m->philos[i].meal_time = start_time;
		m->philos[i].start_time = start_time;
		i++;
	}
}

// Set up the simulation. Initializes all mutexes and threads. If a thread
// couldn't be created, the simulation ends.

void	monitor_begin(t_monitor *m, int arguments[5])
{
	t_philo	*philo;
	int		i;

	i = 0;
	m->thread_count = arguments[0];
	while (i < m->thread_count)
		pthread_mutex_init(&m->mutexes[i++], NULL);
	pthread_mutex_init(&m->print_mutex, NULL);
	i = 0;
	while (i < m->thread_count)
	{
		philo = &m->philos[i];
		philo_init(philo, m, i, arguments);
		if (pthread_create(&philo->thread, NULL, philo_main, philo))
		{
			printf("error: pthread_create failed\n");
			break ;
		}
		m->threads_created++;
		i++;
	}
	start_philo_threads(m);
}

// Main monitoring loop. Continually monitors the philosophers, stopping when
// all meals have been finished, or when a philosopher dies of starvation.

void	monitor_loop(t_monitor *m)
{
	int		i;
	int		finished;
	int64_t	now;

	finished = 0;
	while (m->threads_created == m->thread_count && finished < m->thread_count)
	{
		i = 0;
		finished = 0;
		now = current_time();
		while (i < m->thread_count)
		{
			if (m->philos[i].stop)
				return ;
			if (m->philos[i].meal_count == m->philos[i].meal_limit)
				finished++;
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
	while (i < m->threads_created)
		m->philos[i++].stop = true;
	i = 0;
	while (i < m->threads_created)
		pthread_join(m->philos[i++].thread, NULL);
	i = 0;
	while (i < m->thread_count)
		pthread_mutex_destroy(&m->mutexes[i++]);
	pthread_mutex_destroy(&m->print_mutex);
}
