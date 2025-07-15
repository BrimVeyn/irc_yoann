/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoann <yoann@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 15:28:05 by ylenoel           #+#    #+#             */
/*   Updated: 2025/07/15 18:09:00 by yoann            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <sys/select.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <poll.h>
#include <algorithm>
#include <Client.hpp>
#include <sstream>
#include <map>

using namespace std; // Plus besoin de faire std::map, on peut écrire map direct.

class Server
{
	// Alors, ce qui suit est un alias pour un pointeur sur fonction qui prend en paramètre une référence à un objet client.
	typedef void (Server::*CmdFn)(Client &client, std::string& arg); 

	// Aliases 
	#define ClientMap std::map<int, Client> 
	#define CmdMap std::map<std::string, CmdFn>  

	/* Foncteur : Objet qui agît comme une fonction.
	Ce foncteur = prédicat pour find_if dans remove_clients. 
	On va juste comparer un fd, avec le fd contenu dans une struct pollfd */
	
	struct MatchFd {
		int fd;
		MatchFd(int fd) : fd(fd) {};
		
		//Convention : RHS = Right hand side / LHS = Left hand side
		bool operator()(const pollfd &rhs) { return fd == rhs.fd; }
	};

	private:
	
		int _server_fd;		// socket d'écoute.
		int _port;			// Port du serveur (ex: 6667)
		bool _running;		// État du serveur (On/off)

		void handleNICK(Client &client, std::string& arg);
		void handleUSER(Client &client, std::string& arg);
		void handleREALNAME(Client &client, std::string& arg);
	
		std::vector<struct pollfd> _pollfds;
		ClientMap _db_clients; // Liste des clients connectés.
		CmdMap _cmd_map;

		void setupSocket();
		void listen();
		void acceptNewClient();
		void handleClientMessage(int fd);
		// void closeClient(int fd);
		void close_fds();
		void removeClient(int fd);
		void handleMessage(Client& client, const std::string& msg);
		ClientMap::iterator getClientByFd(const int fd);
	
	public:

		Server(int port);
		~Server();
		void run();
		int getPort() const;
		size_t getClientCount() const;
		const std::vector<pollfd>& getPollFds() const;
		const std::map<int, Client>& getClients() const;
		void printConnectedClients(const Server& server);
};

std::ostream& operator<<(std::ostream& out, const Server& Server);

#endif
