/*******************************************************************************
 * Copyright (c) 2016 QNX Software Systems and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
#include "UDPServer.h"
#include <utility/socket.h>
#include <utility/cc3000_common.h>

UDPServer::UDPServer(uint16_t _port) :
		port(_port), s(-1) {
}

bool UDPServer::begin() {
	if (s >= 0) {
		closesocket(s);
	}

	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s < 0) {
		return false;
	}

	sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = 0;

	if (bind(s, (sockaddr*) &address, sizeof(address)) < 0) {
		return false;
	}

	return true;
}

bool UDPServer::available() {
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 5000;

	fd_set reads;
	FD_ZERO(&reads);
	FD_SET(s, &reads);

	select(s + 1, &reads, NULL, NULL, &timeout);

	if (!FD_ISSET(s, &reads)) {
		return false;
	}

	return true;
}

int UDPServer::read(char *buffer, int size) {
	if (available()) {
		int n = recv(s, buffer, size, 0);
		if (n < 1) {
			return -1;
		}
		return n;
	}
	return -1;
}
