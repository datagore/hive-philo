/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 23:32:05 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/15 10:32:53 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "philo.h"

static int	parse_argument(const char *str)
{
	int	value;

	value = 0;
	while (*str == ' ' || ('\t' <= *str && *str <= '\r'))
		str++;
	while ('0' <= *str && *str <= '9')
		value = value * 10 + *str++ - '0';
	while (*str == ' ' || ('\t' <= *str && *str <= '\r'))
		str++;
	if (*str != '\0' || value <= 0)
	{
		printf("error: invalid argument\n");
		exit(EXIT_FAILURE);
	}
	return (value);
}

static void	*philo_thread(void *arg)
{
	printf("Hi from philo thread\n");
	return (arg);
}

static void	philo_start(t_philo *philo)
{
	int	i;

	i = 0;
	while (i < philo->count)
		pthread_mutex_init(&philo->mutexes[i++], NULL);
	i = 0;
	while (i < philo->count)
	{
		if (pthread_create(&philo->threads[i++], NULL, philo_thread, NULL))
		{
			printf("error: pthread_create failed\n");
			return ;
		}
	}
	i = 0;
	while (i < philo->count)
		pthread_join(philo->threads[i++], NULL);
}

int	main(int argc, char **argv)
{
	t_philo	philo;

	memset(&philo, 0, sizeof(t_philo));
	if (argc != 5 && argc != 6)
		return (printf("usage: %s <count> <die> <eat> [sleep]\n", argv[0]));
	philo.count = parse_argument(argv[1]);
	philo.starve_time = parse_argument(argv[2]);
	philo.eat_time = parse_argument(argv[3]);
	philo.sleep_time = parse_argument(argv[4]);
	philo.target_meals = 0;
	if (argc == 6)
		philo.target_meals = parse_argument(argv[5]);
	philo.threads = malloc(philo.count * sizeof(pthread_t *));
	philo.mutexes = malloc(philo.count * sizeof(pthread_mutex_t));
	if (philo.threads != NULL && philo.mutexes != NULL)
		philo_start(&philo);
	else
		printf("error: can't allocate memory\n");
	free(philo.threads);
	free(philo.mutexes);
}
