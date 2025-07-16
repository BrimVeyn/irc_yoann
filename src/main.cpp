/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ylenoel <ylenoel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 14:03:23 by ylenoel           #+#    #+#             */
/*   Updated: 2025/07/16 17:10:58 by ylenoel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

volatile sig_atomic_t g_running = 1;

void signalHandler(int signum)
{
	(void)signum;
	g_running = 0;
}

int main(int argc, char* argv[])
{
	if(argc != 3)
		exit(EXIT_FAILURE);
	try
	{
		signal(SIGINT, signalHandler); // Capture CTRL+C
		Server test(atoi(argv[1]), argv[2]);
		test.run();
	} catch(const std::exception& e)
	{
		std::cerr << "Server error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

		
	return 0;
}