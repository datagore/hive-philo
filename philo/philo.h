/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 23:36:06 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/15 10:33:03 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>

# define MAX_EVENTS 64

typedef enum e_event_type
{
	EVENT_TOOK_FORK,
	EVENT_EATING,
	EVENT_SLEEPING,
	EVENT_THINKING,
	EVENT_DIED
}	t_event_type;

typedef struct s_event
{
	t_event_type	type;
	int				index;
}	t_event;

typedef struct s_philo
{
	int				count;
	int				starve_time;
	int				eat_time;
	int				sleep_time;
	int				target_meals;
	_Atomic size_t	events_head;
	_Atomic size_t	events_tail;
	t_event			events[MAX_EVENTS];
	pthread_t		*threads;
	pthread_mutex_t	*mutexes;
}	t_philo;

#endif
