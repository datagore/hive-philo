/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 23:36:06 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/22 10:02:44 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdint.h>

# define MAX_EVENTS 1024

typedef enum e_event_type	t_event_type;
typedef struct s_philo		t_philo;

enum e_event_type
{
	EVENT_NONE,
	EVENT_TOOK_FORK,
	EVENT_EATING,
	EVENT_SLEEPING,
	EVENT_THINKING,
	EVENT_DIED
};

struct s_philo
{
	int				count;
	_Atomic int		created;
	_Atomic int		finished;
	int				starve_time;
	int				eat_time;
	int				sleep_time;
	int				max_meals;
	pthread_t		*threads;
	pthread_mutex_t	*mutexes;
	pthread_mutex_t	print_mutex;
	int64_t			start_time;
	_Atomic bool	ended;
	_Atomic size_t	write_pos;
	_Atomic size_t	read_pos;
	_Atomic size_t	events[MAX_EVENTS];
};

#endif
