/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ylenoel <ylenoel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 15:33:22 by ylenoel           #+#    #+#             */
/*   Updated: 2025/07/10 16:59:36 by ylenoel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"

Client::Client() : _fd(-1), _hasNick(false), _hasUser(false) {}

Client::Client(int fd) : _fd(fd), _hasNick(false), _hasUser(false) {}

Client::~Client() {}

int Client::getFd() const {return _fd;}
void Client::setFd(int fd) {_fd = fd;}

const std::string& Client::getNickname() const {return _nickname;}
void Client::setNickname(const std::string& nickname) {_nickname = nickname;}

const std::string& Client::getUsername() const {return _username;}
void Client::setUsername(const std::string& username) {_username = username;}

const std::string& Client::getRealname() const {return _realname;}
void Client::setRealname(const std::string& realname) {_realname = realname;}

bool Client::isRegistered() const {return _hasNick && _hasUser;}
void Client::setHasNick(bool val) {_hasNick = val;}
void Client::setHasUser(bool val) {_hasUser = val;}
void Client::setRegistration(const bool value) {_registered = value;}

void Client::appendToBuffer(const std::string& data) {_buffer += data;}
std::string& Client::getBuffer() {return _buffer;}