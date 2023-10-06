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

#include "StdAfx.h"
#include ".\doom3master.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Doom3Server.h"

Doom3Master::Doom3Master(const idStr &host, const unsigned short port)
	: MasterServer(host, port)
{
}

Doom3Master::~Doom3Master(void)
{
}

struct D3MasterServerData
{
	unsigned char ipAddress[4];
	unsigned short port;
};

struct D3MasterPacket
{
	short check;				// should be -1
	char check2[8];				// should be "servers"
	D3MasterServerData data[1]; // variable number of these - could even be 0

	bool Validate() 
	{
		return (check == -1 || strncmp(check2, "servers", 8) == 0);
	}
};

const char *Doom3Master::GetQueryString(int filter, int &len)
{
	static char queryBuffer[] 
	           = "\xFF\xFFgetServers\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

	len = sizeof(queryBuffer);

	return queryBuffer;
}

bool Doom3Master::Query(int filter)
{
	int len = 0;
	const char *sendbuf = GetQueryString(filter, len);
	size_t packetlen;

	if (!OpenUDPSocket(m_host.c_str(), m_port))
		return false;

	if (SendPacket(sendbuf, len, 0) == SOCKET_ERROR)
	{
		CloseSocket();
		return false;
	}
	
	D3MasterPacket *packet = (D3MasterPacket*)GetPacket(&packetlen);
	while (packet)
	{
		if (!packet->Validate())
		{
			free(packet);

			CloseSocket();
			return false;
		}

		char hostName[20];

		size_t serverCount = (packetlen - (sizeof(D3MasterPacket) - sizeof(D3MasterServerData))) / sizeof(D3MasterServerData);
		for (size_t i=0; i<serverCount; i++)
		{
			sprintf(hostName, "%d.%d.%d.%d", (int)packet->data[i].ipAddress[0], (int)packet->data[i].ipAddress[1], (int)packet->data[i].ipAddress[2], (int)packet->data[i].ipAddress[3]);
			unsigned short port = packet->data[i].port;

			if (IsNewServer(hostName, port))
			{
				GameServer *thisServer = AddServer(hostName, port);
				m_newServers.Append(thisServer);
			}
		}

		free(packet);
		packet = (D3MasterPacket*)GetPacket(&packetlen);
	}

	CloseSocket();

	return true;
}
