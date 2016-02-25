/*******************************************************************************
 * Copyright (c) 2016 QNX Software Systems and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
#ifndef UDPSERVER_H_
#define UDPSERVER_H_

#include <Arduino.h>

class UDPServer {
public:
	UDPServer(uint16_t port);

	bool begin();
	bool available();
	int read(char *buffer, int size);

private:
	uint16_t port;
	int s;
};

#endif /* UDPSERVER_H_ */
