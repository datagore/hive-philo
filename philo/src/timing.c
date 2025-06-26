/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   timing.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 08:18:21 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/26 10:47:07 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>

#include "common.h"

// Get the current time in microseconds, measured from some arbitrary point in
// time.

int64_t	current_time(void)
{
	struct timeval	timeval;

	gettimeofday(&timeval, NULL);
	return (timeval.tv_sec * 1000000 + timeval.tv_usec);
}

// Wait until some specific point in time is reached.

void	wait_until(int64_t target_time)
{
	int64_t	difference;

	difference = target_time - current_time();
	while (difference > 0)
	{
		usleep(difference);
		difference = target_time - current_time();
	}
}

// Wait for a given number of microseconds.

void	wait_for(int64_t duration)
{
	wait_until(current_time() + duration);
}
