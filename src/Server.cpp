/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoann <yoann@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 15:45:29 by ylenoel           #+#    #+#             */
/*   Updated: 2025/07/15 18:09:22 by yoann            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/colors.hpp"


Server::Server(int port) : _port(port), _db_clients()
{
	setupSocket(); // Création du socket serveur + binding
	listen(); // Mise en écoute du socket serveur.
	_cmd_map["NICK"] = &Server::handleNICK;
	_cmd_map["USER"] = &Server::handleUSER;
	_cmd_map["REALNAME"] = &Server::handleREALNAME;
}

Server::~Server()
{
	close_fds();
}

void Server::close_fds()
{
	std::cout << "Shutting down server >>" << std::endl;
	
	// On ferme les clients
	for(size_t i = 0; i < _db_clients.size(); ++i)
		close(_db_clients[i].getFd());

	// On close la socket principale
	if(_server_fd >= 0)
		close(_server_fd);
}

void Server::setupSocket()
{
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(_server_fd == -1)
		throw std::runtime_error("Creating socket failed!");
		
	struct sockaddr_in addr; // Pour bind() il faut remplir une struct addr.
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port); // hostnetwork (converti le port en big endian pour les fonctions qui utilisent le network)
	addr.sin_addr.s_addr = INADDR_ANY; 
	
	if(bind(_server_fd, (sockaddr*)&addr, sizeof(addr)) < 0 && close(_server_fd))
		throw std::runtime_error("Failed to bind server!");

	// On ajoute le socket serveur au vecteur _pollfds
	struct pollfd server_fd = {
		.fd = _server_fd,
		.events = POLLIN, // POLLIN = Mode d'écoute de connexion entrante.
		.revents = 0,
	};
	
	_pollfds.push_back(server_fd);
}

void Server::listen()
{
	if(::listen(_server_fd, SOMAXCONN) < 0)
	{
		throw std::runtime_error("Listen failed!");
		close_fds();
	}
}

void Server::acceptNewClient()
{
	 sockaddr_in client_addr; // Struct pour stocker l'adresse du client
	 socklen_t addr_len = sizeof(client_addr); // taille de la structure
	 
	 int client_fd = accept(_server_fd, (sockaddr*)&client_addr, &addr_len);
	 if(client_fd < 0)
	 {
	 	std::cerr << "Failed to accept client!" << std::endl;
		return;
	 }
	 
	 std::cout << "Nouveau client connecté, fd = " << client_fd 
	 << ", IP: " << inet_ntoa(client_addr.sin_addr)
	 << ", Port: " << ntohs(client_addr.sin_port) << std::endl;
	
	_db_clients.insert(make_pair(client_fd, Client())); // Dans une map il faut insert une pair. On ne peut pas insérer une variable seule.
	_pollfds.push_back((pollfd){
		.fd = client_fd,
		.events = POLLIN,
		.revents = 0,
	});

	

}

void Server::run()
{
	while(true)
	{
		/* Exécution de poll(), qui va remplir les pollfds.revent concerné par des évènements.*/
		// Poll() itère tout seul le long du _pollfds.
		int ret = poll(&_pollfds[0], _pollfds.size(), -1);
		if(ret < 0)
		{
			std::cerr << "Poll failed!" << std::endl;
			continue;
		}
		for(size_t i = 0; i < _pollfds.size(); ++i)
		{
			// Ici, on utilise & et pas && : & est un opérateur de comparaison binaire
			// On l'utilise pour comparer si revent a bien la même valeur que POLLIN. 
			// Si il n'y a pas d'events POLLIN dans le pollfds concerné, on passe au prochain pollfds.
			// POLLIN : État du i/o, prêt à être accéder. Comme pour les flags d'open, il peut-être plusieurs états en même temps.
			if(!(_pollfds[i].revents & POLLIN)) // Si le socket n'est pas prêt à être lu/écrit on passe au prochain socket _pollfds[i].
				continue;
			
			/* Point logique important : 
			Si poll() détecte un event sur le socket serveur, c'est forcément qu'une nouvelle connexion est détectée. 
			Ne pas chercher plus loin. */
			if(_pollfds[i].fd == _server_fd) {
				// Nouvelle connexion
				acceptNewClient();
				continue;
			}
			if (_pollfds[i].revents & (POLLHUP | POLLERR) ){
				std::cout << "Client disconnected (pollhup/pollerr)" << std::endl;
				removeClient(_pollfds[i].fd);
				--i;
				continue;
			}

			int client_fd = _pollfds[i].fd;
			static char buffer[1024];
			int bytes = recv(client_fd, buffer, sizeof(buffer), 0);
			if(bytes <= 0)
			{
				close(client_fd);
				std::cout << "Client disconnected" << std::endl;
				
				// Retirer client des vectors _clients et _pollfds
				removeClient(client_fd);
				--i;
			}
			else
			{
				const ClientMap::iterator maybe_client = getClientByFd(client_fd);
				if (maybe_client == _db_clients.end())
					throw runtime_error("Failed to retreive client information");
				
				cout << "Client sent:" << buffer << endl;	
				handleMessage(maybe_client->second, buffer);
			}
			_pollfds[i].revents = 0;
			printConnectedClients(*this);
			cout << *this << endl;
		}
	}
}



ClientMap::iterator Server::getClientByFd(const int fd) {
	return _db_clients.find(fd);
}

void Server::removeClient(int fd)
{
	_db_clients.erase(fd);
	_pollfds.erase(find_if(_pollfds.begin(), _pollfds.end(), Server::MatchFd(fd)));
	/* Ici, on fait appel au foncteur MatchFd(). On crée une struct MatchFd, et 
	on appelle directement sa fonction membre qui compare son fd, avec celui envoyé par find_if. */
	close(fd);
}

/* Là on est sur un gros morceau. 
1 - Utilisation de stringstream : On a besoin d'extraire des morceaux de message, stringstream nous permet 
d'utiliser des fonctions comme ss qui extraient le premier mot seul, puis les mots suivant à chaque nouvel appel.
2 - _cmd_map : C'est une map de pair cmd (string) / fonction. Ça nous permet d'appeler la fonction adapté pour 
handle la cmd présente dans le msg.

*/

void Server::handleMessage(Client& client, const std::string& msg)
{
	stringstream ss(msg);
	
	string cmdName;
	string arg;
	ss >> cmdName;
	ss >> arg;

	const CmdMap::iterator result = _cmd_map.find(cmdName);
	if (result == _cmd_map.end()) {
		cout << "UNKNOWN COMMAND" << endl;
		return ;
	}

	(this->*(result->second))(client, arg);
	// (this->*fn)(client);
}

int Server::getPort() const {return _port; }

size_t Server::getClientCount() const {return _db_clients.size();}

const vector<pollfd>& Server::getPollFds() const {return _pollfds;}

const map<int, Client>& Server::getClients() const {return _db_clients;}

void Server::printConnectedClients(const Server& server)
{
	const map<int, Client>& clients = server.getClients();

	cout << C_QUARTZ"=== Connected Clients (" << clients.size() << ") ===" C_RESET << endl;

	for(map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it)
	{
		cout << C_QUARTZ"FD: " << it->first << "\n";
		cout << C_QUARTZ << it->second << C_RESET << endl;
	}
}

std::ostream& operator<<(std::ostream& out, const Server& Server)
{
	const vector<pollfd>& pollfds = Server.getPollFds();
	
	out << C_PASTEL_BLUE"=== Server Debug Info ===\n";
	out << "Total fds monitored: " << pollfds.size() << "\n" C_RESET;
	
	for(size_t i = 0; i < pollfds.size(); ++i)
	{
		const pollfd& pfd = pollfds[i];
		out << C_PASTEL_BLUE" [" << i << "] fd: " << pfd.fd
			<< " | events: " << pfd.events
			<< " | revents: " << pfd.revents << "\n" C_RESET;
	}

	return out;
}
