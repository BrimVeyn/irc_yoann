/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ylenoel <ylenoel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 14:03:23 by ylenoel           #+#    #+#             */
/*   Updated: 2025/07/10 15:24:25 by ylenoel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

int main(int argc, char* argv[])
{
	if(argc != 2)
		exit(EXIT_FAILURE);
	try
	{
		Server test(atoi(argv[1]));
		test.run();
	} catch(const std::exception& e)
	{
		std::cerr << "Server error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

		
	return 0;
}