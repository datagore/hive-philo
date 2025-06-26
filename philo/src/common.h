/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 23:36:06 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/26 10:46:26 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMON_H
# define COMMON_H

# include <pthread.h>

typedef struct s_philo	t_philo;	// State for the whole simulation
typedef struct s_diner	t_diner;	// State for one diner
typedef enum e_state	t_state;	// Enumeration type for diner states

struct s_philo
{
	int				count;			// Total number of diners
	int				started;		// Number of threads that were started
	pthread_mutex_t	print_mutex;	// Lock held while printing
	pthread_mutex_t	*mutexes;		// Array of fork mutexes
	t_diner			*diners;		// Array of per-diner data
	int64_t			start_time;		// Timestamp of the simulation start (µs)
};

struct s_diner
{
	int				index;			// 1-based index of the diner
	pthread_t		thread;			// Thread for this diner
	pthread_mutex_t	*fork1;			// First fork to take
	pthread_mutex_t	*fork2;			// Second fork to take
	pthread_mutex_t	*print_mutex;	// Lock held while printing
	_Atomic bool	stop;			// Set to true when simulation should stop
	_Atomic int64_t	meal_time;		// Timestamp of last meal (µs)
	_Atomic int		meal_count;		// Number of meals the diner has had
	int				meal_limit;		// The maximum number of meals to eat
	int64_t			predelay;		// Delay before grabbing the first fork (µs)
	int64_t			time_to_die;	// Time it takes to starve (µs)
	int64_t			time_to_eat;	// Time it takes to eat (µs)
	int64_t			time_to_sleep;	// Time it takes to sleep (µs)
	int64_t			start_time;		// Timestamp of the simulation start (µs)
};

enum e_state
{
	STATE_SLEEPING,		// The diner is sleeping (or just started)
	STATE_THINKING,		// The diner is thinking
	STATE_TAKEN_A_FORK,	// The diner has taken a fork
	STATE_EATING,		// The diner is eating
	STATE_DIED,			// The diner has died of starvation
};

// monitor.c
void	philo_begin(t_philo *philo, int arguments[5]);
void	philo_loop(t_philo *philo);
void	philo_end(t_philo *philo);

// philo.c
void	diner_init(t_diner *diner, t_philo *p, int index, int arguments[5]);
void	*diner_main(void *diner);
void	diner_print(t_diner *diner, t_state state);

// timing.c
int64_t	current_time(void);
void	wait_until(int64_t target_time);
void	wait_for(int64_t duration);

#endif
