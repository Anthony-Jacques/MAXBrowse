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
#include "Quake3Master.h"
#include "Quake3Server.h"

Quake3Master::Quake3Master(const idStr &host, const unsigned short port, const idStr &protocolVersion)
	: MasterServer(host, port)
{
	m_protocol = protocolVersion;
	m_socktimeout.tv_sec = 5;
}

Quake3Master::~Quake3Master(void)
{
}


#pragma pack(1)

struct Q3MasterServerData
{
	unsigned char ipAddress[4];
	unsigned short port;
	char unknown;
};

struct Q3MasterPacket
{
	long check;					// should be -1
	char check2[19];			// should be "getserversResponse\n"
	Q3MasterServerData data[1]; // variable number of these - could even be 0

	bool Validate() 
	{
		return (check == -1 || strncmp(check2, "getserversResponse", 18) == 0);
	}
};

#pragma pack()



#define	Q3_QUERY_FORMAT_STRING	"\xFF\xFF\xFF\xFFgetservers %s empty full"

const char *Quake3Master::GetQueryString(int filter, int &len)
{
	static char queryBuffer[255];
	sprintf(queryBuffer, Q3_QUERY_FORMAT_STRING, m_protocol.c_str());

	len = strlen(Q3_QUERY_FORMAT_STRING) - 2 + m_protocol.Length();

	return queryBuffer;
}

bool Quake3Master::Query(int filter)
{
	int len = 0;
	const char *sendbuf = GetQueryString(filter, len);

	if (!OpenUDPSocket(m_host.c_str(), m_port))
		return false;

	if (SendPacket(sendbuf, len, 0) == SOCKET_ERROR)
	{
		CloseSocket();
		return false;
	}

	while (!RecievePacket())
		;

	CloseSocket();

	return true;
}


bool Quake3Master::RecievePacket()
{
	size_t packetlen;
	Q3MasterPacket *packet = (Q3MasterPacket *)GetPacket(&packetlen);
	if (!packet)
		return true;

	if (!packet->Validate())
	{
		free(packet);
		return false;
	}

	bool finished = false;

	size_t serverCount = (packetlen - (sizeof(Q3MasterPacket) - sizeof(Q3MasterServerData))) / sizeof(Q3MasterServerData);

	for (int i=0; i<serverCount; i++)
	{
		if (strncmp((char*)packet->data[i].ipAddress, "EOT", 4) == 0)
			break;

		if (strncmp((char*)packet->data[i].ipAddress, "EOF", 4) == 0)
		{
			finished = true;
			break;
		}

		char hostName[20];

		sprintf(hostName, "%d.%d.%d.%d", (int)packet->data[i].ipAddress[0], (int)packet->data[i].ipAddress[1], (int)packet->data[i].ipAddress[2], (int)packet->data[i].ipAddress[3]);

		unsigned short port = ntohs(packet->data[i].port);

		if (IsNewServer(hostName, port))
		{
			GameServer *thisServer = AddServer(hostName, port);
			m_newServers.Append(thisServer);
		}
	}

	free(packet);

	return finished;
}
