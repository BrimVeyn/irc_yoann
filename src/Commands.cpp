/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoann <yoann@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 14:33:42 by yoann             #+#    #+#             */
/*   Updated: 2025/07/15 18:23:14 by yoann            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

void Server::handleNICK(Client &client, std::string& arg) {
	client.setNickname(arg);
	cout << "NICK set" << endl;
	if(!client.getUsername().empty() && !client.getNickname().empty() && !client.getRealname().empty()
		&& !client.isRegistered())
	{
		client.setRegistration(true);
		cout << client.isRegistered() << endl;
	}
}

void Server::handleUSER(Client &client, std::string& arg) {
	client.setUsername(arg);
	cout << "USER set" << endl;
	if(!client.getUsername().empty() && !client.getNickname().empty() && !client.getRealname().empty()
		&& !client.isRegistered())
	{
		client.setRegistration(true);
		cout << client.isRegistered() << endl;
	}
}

void Server::handleREALNAME(Client &client, std::string& arg) {
	client.setRealname(arg);
	cout << "REALNAME set" << endl;
	if(!client.getUsername().empty() && !client.getNickname().empty() && !client.getRealname().empty()
		&& !client.isRegistered())
	{
		client.setRegistration(true);
		cout << client.isRegistered() << endl;
	}
}