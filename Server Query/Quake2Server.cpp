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
#include "Quake2Server.h"
#include "PlayerDetails.h"

#pragma pack(1)

typedef struct
{
	long check;
	char check2[5];
	char data[1];
} QUAKE2_PACKET;

#pragma pack()

const char *Quake2Server::GetQueryString(int &len)
{
	static const char queryString[] = "\xFF\xFF\xFF\xFFstatus";
	len = 10;

	return queryString;
}


bool Quake2Server::IsValidPacket(void *ptr) 
{ 
	QUAKE2_PACKET *packet = (QUAKE2_PACKET *)ptr;
	return packet->check == -1 && !strncmp(packet->check2, "print", 5); 
}

char *Quake2Server::GetPacketData(void *ptr) 
{ 
	QUAKE2_PACKET *packet = (QUAKE2_PACKET *)ptr;
	return packet->data;
}


bool Quake2Server::IsTV() const
{
	return false; //m_serverInfo.GetBool("tv");
}

const char* Quake2Server::GetMod() const
{
	return m_serverInfo.GetString(GetBaseGameCvar());
}
