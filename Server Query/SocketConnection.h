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


#ifndef __SOCKETCONNECTION_H__
#define __SOCKETCONNECTION_H__

class SocketConnection
{
public:
	SocketConnection();
	~SocketConnection();

protected:
	bool OpenUDPSocket(const char *host, unsigned short port)
	{
		return OpenSocket(host, port, AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}

	bool OpenTCPSocket(const char *host, unsigned short port)
	{
		return OpenSocket(host, port, AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}

	char *GetPacket(size_t * len);
	int SendPacket(const char * buf, int len, int flags);

	bool IsSocketOpen() { return m_socket == INVALID_SOCKET; }
	void CloseSocket();

	struct timeval m_socktimeout;
	SOCKET m_socket;

private:
	bool OpenSocket(const char *host, unsigned short port, int family, int socktype, int protocol);
};


#endif
