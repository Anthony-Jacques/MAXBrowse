// Licence: 
// 
// This file is part of MaxBrowse.
// 
// MaxBrowse is free software : you can redistribute itand /or modify it under 
// the terms of the GNU General Public License as published by the Free Software 
// Foundation, either version 3 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but 
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
// FITNESS FOR A PARTICULAR PURPOSE.See the GNU General Public License for more details.

#include "stdafx.h"

#include "SocketConnection.h"

SocketConnection::SocketConnection()
{
	m_socktimeout.tv_sec = 3;
	m_socktimeout.tv_usec = 0;
	m_socket = INVALID_SOCKET;
}

SocketConnection::~SocketConnection()
{
	if (IsSocketOpen())
		CloseSocket();
}

bool SocketConnection::OpenSocket(const char *host, unsigned short port, int family, int socktype, int protocol)
{
	struct addrinfo hints = { 0 };
	struct addrinfo *info, *iter;
	char portstr[32];

	sprintf(portstr, "%d", (int)port);

	hints.ai_family = family;
	hints.ai_socktype = socktype;
	hints.ai_protocol = protocol;

	if (getaddrinfo(host, portstr, &hints, &info))
		return false;

	for (iter = info; iter != NULL; iter = iter->ai_next)
	{
		SOCKET sock = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);

		if (sock == INVALID_SOCKET)
			continue;

		if (connect(sock, iter->ai_addr, (int)iter->ai_addrlen) != SOCKET_ERROR)
		{
			freeaddrinfo(info);

			m_socket = sock;
			return true;
		}
		else
		{
			closesocket(sock);
		}
	}

	freeaddrinfo(info);

	return false;
}

void SocketConnection::CloseSocket()
{
	closesocket(m_socket);
	m_socket = INVALID_SOCKET;
}

int SocketConnection::SendPacket(const char * buf, int len, int flags)
{
	return send(m_socket, buf, len, flags);
}

char *SocketConnection::GetPacket(size_t * len)
{
	fd_set set;

	FD_ZERO(&set);
	FD_SET(m_socket, &set);

	if (select(FD_SETSIZE, &set, NULL, NULL, &m_socktimeout) > 0)
	{
 		char *buf = (char *)calloc(10241, sizeof(char));
		int ret = recv(m_socket, buf, 10240, 0);

		if (ret != SOCKET_ERROR && ret != 0)
		{
			*len = ret;
			return (char *)realloc(buf, ret + 1);
		}
		else
		{
#if defined(_DEBUG) && defined(WIN32)
			fprintf(stderr, "%d\n", WSAGetLastError());
#endif
			free(buf);
			return NULL;
		}
	}
	else
		return NULL;
}
