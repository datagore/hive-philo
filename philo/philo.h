/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 23:36:06 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/16 10:03:41 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdint.h>

# define MAX_EVENTS 256

typedef enum e_event_type	t_event_type;
typedef struct s_event		t_event;
typedef struct s_philo		t_philo;

enum e_event_type
{
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
	int				starve_time;
	int				eat_time;
	int				sleep_time;
	int				max_meals;
	pthread_t		*threads;
	pthread_mutex_t	*mutexes;
	pthread_mutex_t	print_mutex;
	int64_t			start_time;
	_Atomic bool	ended;
};

#endif
