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
#include "Quake3Server.h"
#include "PlayerDetails.h"

#include "ICMPPing.h"

#include <Mmsystem.h>

Quake3Server::Quake3Server(const idStr &host, const unsigned short port)
	: GameServer(host, port)
{		
}

Quake3Server::~Quake3Server(void)
{
}

#pragma pack(1)

typedef struct
{
	long check;
	char check2[15];
	char data[1];
} QUAKE3_PACKET;

#pragma pack()

const char *Quake3Server::GetQueryString(int &len)
{
	static const char queryString[] = "\xFF\xFF\xFF\xFFgetstatus";
	len = 13;

	return queryString;
}

bool Quake3Server::Query()
{
	if (!OpenUDPSocket(m_host.c_str(), m_port))
		return false;

	m_ping = PING_TIMEOUT;
	m_serverInfo.SetInt("ping", m_ping);
	m_players.DeleteContents(true);
	m_serverInfo.Set("players", va("%d / %d", m_players.Num(), m_serverInfo.GetInt("sv_maxclients")));
	m_numBots = 0;

	DWORD startTime = timeGetTime();

	int queryLen;
	const char *queryString = GetQueryString(queryLen);
	if (SendPacket(queryString, queryLen, 0) == SOCKET_ERROR)
	{
		CloseSocket();
		return false;
	}

	size_t packetlen;
	char *packet = GetPacket(&packetlen);
	if (!packet)
	{
		CloseSocket();
		return false;
	}

	DWORD endTime = timeGetTime();

	m_ping = ICMPPing::GetInstance().PingHost(m_host.c_str());
	if (m_ping == -1)
		m_ping = endTime - startTime;

	CloseSocket();

//	if (packet->check != -1 || strncmp(packet->check2, "statusResponse", 14))
	if (!IsValidPacket(packet))
	{
		free(packet);
		return false;
	}

	char *iter = GetPacketData(packet);
	char *end = ((char *)packet) + packetlen;

	while (iter < end)
	{
		if (*iter == '\\')
		{
			idStr key = ParseString(&iter);

			if (iter < end)
			{
				idStr value = ParseString(&iter);
				if (key.Length() == 0)
					break;

				value.RemoveEscapes(true);

				TranslateServerinfo(key, value);
				m_serverInfo.Set(key.c_str(), value.c_str());
			} 
		}
		else if (*iter == '\n')
		{
			idStr playerInfo = ParseString(&iter);
			if (playerInfo.Length() > 0)
			{
				PlayerDetails *player = new PlayerDetails;
				ParsePlayerData(playerInfo, player);

				m_players.Append(player);
			}
		}
		else iter++;
	}

	m_serverInfo.SetInt("ping", m_ping);

	if (idStr::Length(m_serverInfo.GetString(GetBaseGameCvar())) == 0)
		m_serverInfo.Set(GetBaseGameCvar(), GetBaseGame());

	if (m_numBots > 0)
		m_serverInfo.Set("players", va("%d+%d / %d", m_players.Num() - m_numBots, m_numBots, m_serverInfo.GetInt(GetMaxClientsCVar())));
	else
		m_serverInfo.Set("players", va("%d / %d", m_players.Num(), m_serverInfo.GetInt(GetMaxClientsCVar())));

	TranslateGameType();
	
	free(packet);

	return true;
}

const char *Quake3Server::GetServerName() const
{
	return m_serverInfo.GetString("sv_hostname");
}


void Quake3Server::ParsePlayerData(idStr str, PlayerDetails *player)
{
	char name[128];

	int ping = 0;
	int score = 0;
	sscanf(str.c_str(), "%d %d \"%[^\"]", &score, &ping, name);

	idStr strippedName = name;
	strippedName.RemoveEscapes(true);

	player->SetProperty("name", strippedName);
	player->SetProperty("ping", va("%d", ping));
	player->SetProperty("score", va("%d", score));

	if (ping == 0)
	{
		player->SetProperty("bot", "1");
		m_numBots++;
	}
}


idStr Quake3Server::ParseString(char **data)
{
	size_t len;

	if ((*data)[0] != '\\' && (*data)[0] != '\n')
		return "";

	(*data)++;

	len = strcspn(*data, "\\\n");
	if (len > 0)
	{
		
		idStr str(*data, 0, len);

		(*data) += len;

		return str;
	}
	else
		return "";
}

const char* Quake3Server::GetMapName() const
{
	return m_serverInfo.GetString("mapname");
}

const char* Quake3Server::GetGametype() const
{
	return m_serverInfo.GetString("g_gametype");
}

void Quake3Server::TranslateServerinfo(const idStr key, idStr &value)
{
	if (key == "protocol")
	{
		if (value == "68")
			value = "Q3 1.32";
		else if (value == "84")
			value = "ET 2.6";
		else if (value == "83")
			value = "ET 2.5";
		else 
			value = va("Protocol %s", value.c_str());
	}
}

void Quake3Server::TranslateGameType()
{
	idStr value = m_serverInfo.GetString("g_gametype");
	int gameType = atoi(value.c_str());

	switch (gameType)
	{
	case 0: value = "FFA"; break;
	case 1: value = "Tourney"; break;
	case 2: value = "SP"; break;
	case 3: value = "TDM"; break;
	case 4: value = "CTF"; break;
	case 5: value = "1F CTF"; break;
	case 6: value = "Obelisk"; break;
	case 7: value = "Harvester"; break;
	default: break;
	}

	idStr mod = m_serverInfo.GetString("gamename");

	if (mod.ICmp("arena") == 0)
	{
		if (gameType == 8)
			value = "RA3";
	}
	else if (mod.ICmp("excessiveplus") == 0)
	{
		if (gameType == 8)
			value == "Freezetag";
	}
	else if ((mod.ICmp("cpma") == 0) || (mod.ICmp("osp") == 0))
	{
		switch (gameType)
		{
		case 2: value = "DA"; break;
		case 5: value = "CA"; break;
		case 6: value = "Freezetag"; break;
		case 7: value = "CTFS"; break;
		case 8: value = "NTF"; break;
		case 9: value = "2v2"; break;
		case -1: value = "Hoonymode"; break;
		default: break;
		}
	}
	else if (mod.ICmp("q3ut4") == 0)
	{
		value = va("%d", gameType);
		switch (gameType)
		{
		case 0: value = "FFA"; break;
		case 1: value = "Tourney"; break;
		case 2: value = "SP"; break;
		case 3: value = "TDM"; break;
		case 4: value = "Team Survivor"; break;
		case 5: value = "Follow The Leader"; break;
		case 6: value = "Capture And Hold"; break;
		case 7: value = "CTF"; break;
		case 8: value = "Bomb and Difuse"; break;
		default: break;
		}
	}
	else if (mod.ICmp("threewave") == 0)
	{
		value = va("%d", gameType);
		switch (gameType)
		{
		case 5: value = "1F CTF"; break;
		case 11: value = "Arena"; break;
		case 9: value = "CaptureStrike"; break;
		case 10: value = "Classic CTF"; break;
		default: break;
		}
	}

	m_serverInfo.Set("g_gametype", value.c_str());
}

bool Quake3Server::IsTV() const
{
	return (idStr::ICmp(m_serverInfo.GetString("game"), "GTV3") == 0);
}

bool Quake3Server::IsValidPacket(void *ptr) 
{ 
	QUAKE3_PACKET *packet = (QUAKE3_PACKET *)ptr;
	return packet->check == -1 && !strncmp(packet->check2, "statusResponse", 14); 
}

char *Quake3Server::GetPacketData(void *ptr) 
{ 
	QUAKE3_PACKET *packet = (QUAKE3_PACKET *)ptr;
	return packet->data;
}

const char* Quake3Server::GetMod() const
{
	return m_serverInfo.GetString("gamename");
}

