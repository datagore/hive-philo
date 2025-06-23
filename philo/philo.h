/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 23:36:06 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/23 12:36:46 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <limits.h>
# include <pthread.h>
# include <stdint.h>

typedef enum e_state	t_state;
typedef struct s_philo	t_philo;

enum e_state
{
	STATE_SLEEPING,
	STATE_THINKING,
	STATE_TOOK_FORK1,
	STATE_TOOK_FORK2,
	STATE_EATING,
	STATE_DIED = UINT64_MAX
};

struct s_philo
{
	int					count;		// The total number of philosophers.
	int					starve_time;// Starvation time in microseconds.
	int					eat_time;	// Eating time in microseconds.
	int					sleep_time;	// Sleeping time in microseconds.
	int					max_meals;	// Maximum number of meals to eat.
	_Atomic int			created;	// How many threads have been created.
	_Atomic int			finished;	// How many have finished all meals.
	_Atomic int			died;		// How many have died.
	pthread_t			*threads;	// Array of threads per philosopher.
	pthread_mutex_t		*mutexes;	// Array of mutexes per fork.
	int64_t				start_time;	// Timestamp of the start of the simulation.
	_Atomic uint64_t	*states;	// Array of philosopher states.
	_Atomic int64_t		*meal_times;// Array of last meal times.
};

#endif
