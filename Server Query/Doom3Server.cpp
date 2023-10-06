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
#include "doom3server.h"
#include "ETQWServer.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Mmsystem.h>

#include "ICMPPing.h"

#include "PlayerDetails.h"

#define	PROTOCOL_MAJOR_MASK		0x00007fff
#define	PROTOCOL_MINOR_MASK		0x7fff0000
#define	PROTOCOL_MAJOR_SHIFT	0
#define	PROTOCOL_MINOR_SHIFT	16

#define	PROTOCOL_GERMAN_MASK	0x00008000

#define MAX_CLIENTS				32

#pragma pack(1)

typedef struct
{
	short check;
	char check2[12];
	int challengeId;
	int protocol;
	char pack;
	char data[1];
} DOOM3_PACKET;

#pragma pack()

char *Doom3Server::ParseString(char **data)
{
	char *value = *data;

	*data = strchr(value, '\0') + 1;
	return value;
}

Doom3Server::Doom3Server(const idStr &host, const unsigned short port)
	: GameServer(host, port)
{		
}

Doom3Server::~Doom3Server(void)
{
}

bool Doom3Server::Query()
{
	const char *host = m_host.c_str();
	if (!OpenUDPSocket(host, m_port))
		return false;

	m_ping = PING_TIMEOUT;
	m_serverInfo.SetInt("ping", m_ping);
	m_players.DeleteContents(true);

	m_numBots = 0;
	if (m_serverInfo.GetBool("si_tv"))
		m_serverInfo.Set("players", va("%d / %d", m_serverInfo.GetInt("si_viewers"), m_serverInfo.GetInt("si_maxviewers")));
	else
		m_serverInfo.Set("players", va("%d / %d", m_players.Num(), m_serverInfo.GetInt("si_maxplayers")));

	DWORD startTime = timeGetTime();

	if (SendPacket("\xFF\xFFgetInfo\x00\x01\x00\x00\x00", 14, 0) == SOCKET_ERROR)
	{
		CloseSocket();
		return false;
	}

	size_t packetlen;
	DOOM3_PACKET *packet = (DOOM3_PACKET *) GetPacket(&packetlen);
	if (!packet)
	{
		CloseSocket();
		return false;
	}

	DWORD endTime = timeGetTime();

	m_ping = ICMPPing::GetInstance().PingHost(host);
	if (m_ping == -1)
		m_ping = endTime - startTime;

	CloseSocket();

	if (packet->check != -1 || strncmp(packet->check2, "infoResponse", 12))
	{
		free(packet);
		return false;
	}

	return ParseResponse(packet, packetlen);
}

bool Doom3Server::ParseResponse(void *rawPacket, int packetlen)
{
	DOOM3_PACKET *packet = (DOOM3_PACKET *)rawPacket;
	char *iter = packet->data;
	char *end = ((char *)packet) + packetlen;

	while (iter < end)
	{
		char *key = ParseString(&iter);

		if (iter < end)
		{
			idStr value = ParseString(&iter);
			if (strlen(key) == 0)
				break;

			value.RemoveEscapes(false);

			TranslateServerinfo(key, value);

			m_serverInfo.Set(key, value.c_str());
		} 
	}

	m_serverInfo.SetInt("ping", m_ping);

	if (idStr::Length(m_serverInfo.GetString("fs_game")) == 0)
		m_serverInfo.Set("fs_game", GetBaseGame());

	while (iter < end && iter[0] != MAX_CLIENTS)
	{
		PlayerDetails *player  = new PlayerDetails;

		iter = ParsePlayerData(iter, end, player);

		m_players.Append(player);
	}
	// after this there is more data, but dont particularly see the point in it:
	// byte - MAX_CLIENTS
	// long - OS mask
	// long - have code pak lists (?)

	if (m_serverInfo.GetBool("si_tv"))
		m_serverInfo.Set("players", va("%d / %d", m_serverInfo.GetInt("ri_numviewers"), m_serverInfo.GetInt("ri_maxviewers")));
	else
		m_serverInfo.Set("players", va("%d / %d", m_players.Num(), m_serverInfo.GetInt("si_maxplayers")));

	int protocol = ntohl(packet->protocol);
	idStr protocolVal = va("%d.%d%s", (protocol & PROTOCOL_MAJOR_MASK) >> PROTOCOL_MAJOR_SHIFT, 
										(protocol & PROTOCOL_MINOR_MASK)  >> PROTOCOL_MINOR_SHIFT,
										(protocol & PROTOCOL_GERMAN_MASK) ? " DE" : "");

	
	TranslateServerinfo("protocol", protocolVal);

	m_serverInfo.Set("protocol", protocolVal.c_str());

	free(packet);

	return true;
}

char *Doom3Server::ParsePlayerData(char *pos, char *packetEnd, PlayerDetails *player)
{
	int entityNum = *pos;
	player->SetProperty("entitynum", va("%d", entityNum));
	pos++;					

	if (pos < packetEnd)
	{
		int ping = (int)(*((short *)pos));
		player->SetProperty("ping", va("%d", ping));
		pos += sizeof(short);
	}

	if (pos < packetEnd)
		pos += 4;			// skip rate

	if (pos < packetEnd)
	{
		idStr name = ParseString(&pos);
		name.RemoveEscapes(false);

		player->SetProperty("name", name);
	}

	return pos;
}


const char *Doom3Server::GetServerName() const
{
	return m_serverInfo.GetString("si_name");
}

const char *Doom3Server::GetMapName() const
{
	return m_serverInfo.GetString("si_map");
}

const char *Doom3Server::GetGametype() const
{
	return m_serverInfo.GetString("si_gametype");
}


void Doom3Server::TranslateServerinfo(const idStr key, idStr &value)
{
	if (key == "protocol")
	{
		if (value == "1.33")
			value = "D3 1.0";
		else if (value == "1.34")
			value = "D3 1.1 Beta";
		else if (value == "1.35")
			value = "D3 1.1";
		else if (value == "1.39")
			value = "D3 1.2 XP";
		else if (value == "1.40")
			value = "D3 1.3";
		else if (value == "1.41")
			value = "D3 1.3.1";
		else if (value == "3.40")
			value = "Prey 1.0";
		else if (value == "3.40")
			value = "Prey 1.1";
		else if (value == "3.42")
			value = "Prey 1.2 to 1.4";
		else 
			value = va("Protocol %s", value.c_str());
	}
}
bool Doom3Server::IsTV() const
{
	return m_serverInfo.GetBool("si_tv");
}

const char* Doom3Server::GetMod() const
{
	return m_serverInfo.GetString("fs_game");
}