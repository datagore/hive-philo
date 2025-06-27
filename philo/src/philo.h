/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 23:36:06 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/26 20:51:13 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMON_H
# define COMMON_H

# include <pthread.h>
# include <stdbool.h>

# define MONITOR_RATE 500	// Rate at which the monitor thread runs (µs)
# define START_DELAY 10000	// Delay before threads start running (µs)
# define THINK_DELAY 250	// Small delay added to the think stage (µs)
# define MAX_WAIT_STEP 500	// Maximum time a thread will wait in one go (µs)

typedef struct s_monitor	t_monitor;	// State for the monitor thread
typedef struct s_philo		t_philo;	// State for one philosopher
typedef enum e_state		t_state;	// Philosopher state type

struct s_monitor
{
	int				thread_count;		// Total number of threads
	int				threads_created;	// Number of threads that were started
	pthread_mutex_t	print_mutex;		// Lock held while printing
	pthread_mutex_t	*mutexes;			// Array of fork mutexes
	t_philo			*philos;			// Array of per-philosopher data
};

struct s_philo
{
	int				index;			// 1-based index of the philosopher
	pthread_t		thread;			// Thread for this philosopher
	pthread_mutex_t	*fork1;			// First fork to take
	pthread_mutex_t	*fork2;			// Second fork to take
	pthread_mutex_t	*print_mutex;	// Lock held while printing
	_Atomic int64_t	start_time;		// Timestamp of the simulation start (µs)
	_Atomic bool	stop;			// Set to true when simulation should stop
	_Atomic int64_t	meal_time;		// Timestamp of last meal (µs)
	_Atomic int		meal_count;		// Number of meals the philosopher has had
	int				meal_limit;		// The maximum number of meals to eat
	int64_t			predelay;		// Delay before grabbing the first fork (µs)
	int64_t			time_to_die;	// Time it takes to starve (µs)
	int64_t			time_to_eat;	// Time it takes to eat (µs)
	int64_t			time_to_sleep;	// Time it takes to sleep (µs)
};

enum e_state
{
	STATE_SLEEPING,		// The philosopher is sleeping (or just started)
	STATE_THINKING,		// The philosopher is thinking
	STATE_TAKEN_A_FORK,	// The philosopher has taken a fork
	STATE_EATING,		// The philosopher is eating
	STATE_DIED,			// The philosopher has died of starvation
};

// monitor.c
void	monitor_begin(t_monitor *monitor, int arguments[5]);
void	monitor_loop(t_monitor *monitor);
void	monitor_end(t_monitor *monitor);

// philo.c
void	philo_init(t_philo *philo, t_monitor *m, int index, int arguments[5]);
void	*philo_main(void *philo);
void	philo_print(t_philo *philo, t_state state);

// timing.c
int64_t	current_time(void);
void	wait_until(t_philo *philo, int64_t target_time);
void	wait_for(t_philo *philo, int64_t duration);

#endif
