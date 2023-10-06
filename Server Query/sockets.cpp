#include "stdafx.h"


struct timeval socktimeout = { 3, 0 };

SOCKET getsock(const char *host, unsigned short port, int family, int socktype, int protocol)
{
	struct addrinfo hints = { 0 };
	struct addrinfo *info, *iter;
	char portstr[32];

	sprintf(portstr, "%d", (int)port);

	hints.ai_family = family;
	hints.ai_socktype = socktype;
	hints.ai_protocol = protocol;

	if (getaddrinfo(host, portstr, &hints, &info))
		return INVALID_SOCKET;

	for (iter = info; iter != NULL; iter = iter->ai_next)
	{
		SOCKET sock = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);

		if (sock == INVALID_SOCKET)
			continue;

		if (connect(sock, iter->ai_addr, (int)iter->ai_addrlen) != SOCKET_ERROR)
		{
			freeaddrinfo(info);

			return sock;
		}
		else
			closesocket(sock);
	}

	freeaddrinfo(info);

	return INVALID_SOCKET;
}

char *getpacket(SOCKET s, size_t * len)
{
	fd_set set;

	FD_ZERO(&set);
	FD_SET(s, &set);

	if (select(FD_SETSIZE, &set, NULL, NULL, &socktimeout) > 0)
	{
 		char *buf = (char *)calloc(10241, sizeof(char));
		int ret = recv(s, buf, 10240, 0);

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
