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
#include "WarsowServer.h"
#include "PlayerDetails.h"

#pragma pack(1)

typedef struct
{
	long check;
	char check2[12];
	char data[1];
} WARSOW_PACKET;

#pragma pack()

const char *WarsowServer::GetQueryString(int &len)
{
	static const char queryString[] = "\xFF\xFF\xFF\xFFgetinfo";
	len = 11;

	return queryString;
}


bool WarsowServer::IsValidPacket(void *ptr) 
{ 
	WARSOW_PACKET *packet = (WARSOW_PACKET *)ptr;
	return packet->check == -1 && !strncmp(packet->check2, "infoResponse", 12); 
}

char *WarsowServer::GetPacketData(void *ptr) 
{ 
	WARSOW_PACKET *packet = (WARSOW_PACKET *)ptr;
	return packet->data;
}


bool WarsowServer::IsTV() const
{
	return m_serverInfo.GetBool("tv");
}


void WarsowServer::ParsePlayerData(idStr str, PlayerDetails *player)
{
	if (IsTV())
	{
		char name[128] = { '\0' };
		char map[128] = { '\0' };

		int ping = 0;
		int id = 0;
		sscanf(str.c_str(), "%d \"%[^\"]\" %d \"%[^\"]\"", &id, name, &ping, map);

		idStr strippedName = name;
		strippedName.RemoveEscapes(true);

		player->SetProperty("name", strippedName);
		player->SetProperty("score", map);
		player->SetProperty("ping", va("%d", id));
	}
	else
	{
		Quake3Server::ParsePlayerData(str, player);
	}
}

const char* WarsowServer::GetMod() const
{
	return m_serverInfo.GetString("fs_game");
}
