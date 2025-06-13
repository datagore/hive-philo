/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 23:32:05 by abostrom          #+#    #+#             */
/*   Updated: 2025/06/13 23:42:19 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>

#include "philo.h"

static int	parse_argument(const char *s)
{
	int	value;

	value = 0;
	while (*s == ' ' || ('\t' <= *s && *s <= '\r'))
		s++;
	while ('0' <= *s && *s <= '9')
		value = value * 10 + *s++ - '0';
	while (*s == ' ' || ('\t' <= *s && *s <= '\r'))
		s++;
	if (*s != '\0')
	{
		printf("Error: invalid argument\n");
		exit(EXIT_FAILURE);
	}
	return (value);
}

int	main(int argc, char **argv)
{
	t_philo philo;

	if (argc != 5 && argc != 6)
		return printf("usage: %s <count> <die> <eat> [sleep]\n", argv[0]);
	philo.count = parse_argument(argv[1]);
	philo.starve_time = parse_argument(argv[2]);
	philo.eat_time = parse_argument(argv[3]);
	philo.sleep_time = parse_argument(argv[4]);
	philo.target_meals = 0;
	if (argc == 6)
		philo.target_meals = parse_argument(argv[5]);
}
