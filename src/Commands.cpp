/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ylenoel <ylenoel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 14:33:42 by yoann             #+#    #+#             */
/*   Updated: 2025/07/16 16:40:45 by ylenoel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

void Server::handleNICK(Client &client, std::string &arg)
{
	if (!client.getHasPassword())
	{
		std::ostringstream oss;
		oss << ":" << getServerHostName() << " 451 * :You have not registered\r\n";
		sendToClient(client, oss.str());
		return;
	}

	stringstream ss(arg);
	string nickname;
	ss >> nickname;

	if (nickname.empty())
	{
		std::ostringstream oss;
		oss << ":" << getServerHostName() << " 431 * :No nickname given\r\n";
		sendToClient(client, oss.str());
		return;
	}

	// if (isNicknameTaken(nickname)) // À implémenter : vérifie si le nick est déjà utilisé
	// {
	// 	std::ostringstream oss;
	// 	oss << ":" << getServerHostName() << " 433 * " << nickname << " :Nickname is already in use\r\n";
	// 	sendToClient(client, oss.str());
	// 	return;
	// }

	client.setNickname(nickname);

	if (!client.getUsername().empty() && !client.getRealname().empty() && !client.isRegistered())
	{
		client.setRegistration(true);

		std::ostringstream welcome;
		welcome << ":" << getServerHostName() << " 001 " << client.getNickname()
				<< " :Welcome to the Internet Relay Network " << client.getNickname()
				<< "!" << client.getUsername() << "\r\n";

		sendToClient(client, welcome.str());
	}
}


void Server::handlePASS(Client &client, std::string& arg)
{
	string hostname = getServerHostName();
	string nick = client.getNickname().empty() ? "*" : client.getNickname();
	stringstream ss(arg);
	string pass;
	ss >> pass;
	
	if (client.getHasPassword())
	{
		std::ostringstream oss;
		oss << ":" << hostname << " 462 " << nick << " :You may not reregister\r\n";
		sendToClient(client, oss.str());
		return;
	}
	else if (pass.empty())
	{
		std::ostringstream oss;
		oss << ":" << hostname << " 461 " << nick << " PASS :Not enough parameters\r\n";
		sendToClient(client, oss.str());
		return;
	}
	else if (pass == this->getPassword())
	{
		client.setHasPassword(true);
		sendToClient(client, ":" + hostname + " NOTICE " + nick + " :Password accepted. You are now connected.\r\n");
		sendToClient(client, ":" + hostname + " NOTICE " + nick + " :Please register using NICK and USER.\r\n");
	}
	else
	{
		std::ostringstream oss;
		oss << ":" << hostname << " 464 " << nick << " :Password incorrect\r\n";
		sendToClient(client, oss.str());
		removeClient(client.getFd());  // Tu peux aussi fermer le socket ici si nécessaire
	}
}


void Server::handleUSER(Client &client, std::string& arg) {
	if (!client.getHasPassword()) {
		std::ostringstream oss;
		oss << ":" << getServerHostName() << " 451 * :You have not registered\r\n";
		sendToClient(client, oss.str());
		return;
	}

	std::stringstream ss(arg);
	std::string username, hostname, servername, realname;

	ss >> username >> hostname >> servername;

	// Lire le reste de la ligne comme realname (peut contenir des espaces)
	std::string rest;
	std::getline(ss, rest);

	// Supprimer le ':' s'il existe au début
	if (!rest.empty() && rest[0] == ':')
		rest.erase(0, 1);

	realname = rest;

	if (username.empty() || realname.empty()) {
		sendToClient(client, "461 USER :Not enough parameters\r\n");
		return;
	}

	client.setUsername(username);
	client.setRealname(realname);

	std::cout << "USER set: " << username << ", REALNAME: " << realname << std::endl;

	// Vérifie si l'utilisateur peut maintenant être considéré comme "registered"
	if (!client.getNickname().empty()
		&& !client.getUsername().empty()
		&& !client.getRealname().empty()
		&& !client.isRegistered())
	{
		client.setRegistration(true);
		std::cout << "Client is now registered." << std::endl;
	}
}