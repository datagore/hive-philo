/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   timing.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 08:18:21 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/27 11:38:22 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>

#include "philo.h"

// Get the current time in microseconds, measured from some arbitrary point in
// time.

int64_t	current_time(void)
{
	struct timeval	timeval;

	gettimeofday(&timeval, NULL);
	return (timeval.tv_sec * 1000000 + timeval.tv_usec);
}

// Wait until some specific point in time is reached, or until the simulation
// ends for any reason. If the wait time is greater than an arbitrary threshold,
// it is broken into smaller steps, to avoid long waits when the simulation
// ends.

void	wait_until(t_philo *philo, int64_t target_time)
{
	int64_t	difference;

	difference = target_time - current_time();
	while (difference > 0 && !philo->stop)
	{
		if (difference > MAX_WAIT_STEP)
			difference = MAX_WAIT_STEP;
		usleep(difference);
		difference = target_time - current_time();
	}
}

// Wait for a given number of microseconds.

void	wait_for(t_philo *philo, int64_t duration)
{
	wait_until(philo, current_time() + duration);
}
