/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 23:36:06 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/25 23:46:27 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdbool.h>

typedef struct s_philo	t_philo;	// State for the whole simulation
typedef struct s_diner	t_diner;	// State for one diner
typedef enum e_state	t_state;	// Enumeration type for diner states

struct s_philo
{
	bool			stop;		// Set to true when simulation ends
	int				count;		// Total number of diners
	int				started;	// Number of threads that were started
	pthread_mutex_t	*mutexes;	// Array of fork mutexes
	t_diner			*diners;	// Array of per-diner data
};

struct s_diner
{
	int				index;			// 1-based index of the diner
	pthread_t		thread;			// Thread for this diner
	pthread_mutex_t	*fork1;			// First fork to take
	pthread_mutex_t	*fork2;			// Second fork to take
	_Atomic bool	stop;			// Set to true when simulation ends
	_Atomic int64_t	meal_time;		// Timestamp of last meal (µs)
	_Atomic int		meal_count;		// Number of meals the diner has had
	int				meal_limit;		// The maximum number of meals to eat
	int64_t			predelay;		// Delay before grabbing the first fork (µs)
	int64_t			time_to_die;	// Time it takes to starve (µs)
	int64_t			time_to_eat;	// Time it takes to eat (µs)
	int64_t			time_to_sleep;	// Time it takes to sleep (µs)
};

enum e_state
{
	STATE_SLEEPING,			// The diner is sleeping (or just started)
	STATE_THINKING,			// The diner is thinking
	STATE_TAKEN_FORK1,		// The diner has taken its first fork
	STATE_TAKEN_FORK2,		// The diner has taken its second fork
	STATE_EATING,			// The diner is eating
	STATE_MAX,				// Not a real state, just used for cycling states
	STATE_DIED = STATE_MAX,	// Only used for printing death messages
};

int64_t	current_time(void);
void	print_state(t_philo *philo, t_state state, int index);
void	parse_arguments(int argc, char **argv, int arguments[5]);

#endif
