/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ylenoel <ylenoel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 15:28:05 by ylenoel           #+#    #+#             */
/*   Updated: 2025/07/16 17:01:40 by ylenoel          ###   ########.fr       */
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
#include <cerrno>
#include <csignal>

extern volatile sig_atomic_t g_running;
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

		string _serverHostName; // Nom du serveur
		int _server_fd;		// socket d'écoute.
		int _port;			// Port du serveur (ex: 6667)
		bool _running;		// État du serveur (On/off)
		string _password;	// Password pour se connecter au réseau

		void handleNICK(Client &client, std::string& arg);
		void handleUSER(Client &client, std::string& arg);
		void handlePASS(Client &client, std::string& arg);
	
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
		bool sendToClient(const Client& client, const std::string& msg);
		bool isNicknameTaken(const std::string& nickname) const;
		
	public:

		Server(int port, string password);
		~Server();
		void run();
		int getPort() const;
		string getServerHostName() const;
		size_t getClientCount() const;
		string getPassword() const;
		const std::vector<pollfd>& getPollFds() const;
		const std::map<int, Client>& getClients() const;
		void printConnectedClients(const Server& server);
};

std::ostream& operator<<(std::ostream& out, const Server& Server);

#endif
