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
#include ".\ETQWServer.h"

#include "PlayerDetails.h"

#define	PROTOCOL_MAJOR_MASK		0x7fff0000
#define	PROTOCOL_MINOR_MASK		0x00007fff
#define	PROTOCOL_MAJOR_SHIFT	16
#define	PROTOCOL_MINOR_SHIFT	0

#define	PROTOCOL_GERMAN_MASK	0x00008000

#define MAX_CLIENTS				32

#pragma pack(1)

typedef struct
{
	short check;
	char check2[13];
	int challengeId;
	int challengeId2;
	int protocol;
	int size;
	char data[1];
} ETQW_PACKET;

#pragma pack()

ETQWServer::ETQWServer(const idStr &host, const unsigned short port)
	: Doom3Server(host, port)
{
}


ETQWServer::~ETQWServer(void)
{
}



void ETQWServer::TranslateServerinfo(const idStr key, idStr &value)
{
	if (key == "protocol")
	{
		if (value == "10.21")
			value = "ETQW v1.5";
		else if (value == "10.20")
			value = "ETQW v1.5 Beta";
		else if (value == "12.19")
			value = "ETQW v1.4 DE demo";
		else if (value == "10.19")
			value = "ETQW v1.4";
		else if (value == "10.18")
			value = "ETQW v1.3";
		else if (value == "10.17")
			value = "ETQW v1.2";
		else if (value == "10.16")
			value = "ETQW v1.1";
		else if (value == "10.15")
			value = "ETQW v1.0";
		else if (value == "13.10")
			value = "Open Beta 1";
		else if (value == "13.12")
			value = "Open Beta 2";
		else
			value = va("Protocol %s", value.c_str());
	}
	else if (key == "si_rules")
	{
		std::string test;
		
//		if (value.ICmpn("sdGameRules", 11) == 0)
//			value = value.Right(value.Length() - 11);
			if (value.Find("GameRules", false) != -1)
			{
				for (int i=7; i<value.Length() - 2; i++)
				{
					if (value[i] == 'e' && value[i+1] == 's')
					{
						value = value.Right(value.Length() - (i+2));
						break;
					}
				}	
			}
	}
	else if (key == "si_map")
	{
		value.Replace("maps/", "");
		value.Replace(".entities", "");
	}
}

#define		MAX_CLIENTS		32

bool ETQWServer::ParseResponse(void *rawPacket, int packetlen)
{
	ETQW_PACKET *packet = (ETQW_PACKET *)rawPacket;

	int protocol = packet->protocol;
	idStr protocolVal = va("%d.%d", (protocol & PROTOCOL_MAJOR_MASK) >> PROTOCOL_MAJOR_SHIFT, 
										(protocol & PROTOCOL_MINOR_MASK) >> PROTOCOL_MINOR_SHIFT);

	idStr translatedProtocol = protocolVal;
	TranslateServerinfo("protocol", translatedProtocol);

	m_serverInfo.Set("protocol", translatedProtocol.c_str());

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

	m_numBots = 0;
	while (iter < end && iter[0] != MAX_CLIENTS)
	{
		PlayerDetails *player  = new PlayerDetails;

		int entityNum = *iter;
		player->SetProperty("entitynum", va("%d", entityNum));

		iter++;					

		if (iter < end)
		{
			int ping = (int)(*((short *)iter));
			player->SetProperty("ping", va("%d", ping));
			iter += sizeof(short);
		}

//		if (iter < end)
//			iter += 2;			// skip rate

//		if (iter < end)
//		{
//			int score = (int)(*((short *)iter));
//			player->SetProperty("score", va("%d", score));
//			iter += sizeof(short);
//		}

		if (iter < end)
		{
			idStr name = ParseString(&iter);
			name.RemoveEscapes(false);
			player->SetProperty("name", name);
		}

		if (protocolVal.ICmp("10.15") != 0 && protocolVal.ICmp("10.16") != 0)
		{
			if (iter < end)
				iter += 1;			// skip unknown

			if (iter < end)
			{
				idStr clan = ParseString(&iter);
				clan.RemoveEscapes(false);

				player->SetProperty("clan", clan);
			}
		}

		if (iter < end)
		{
			unsigned char *isBotPtr = (unsigned char*)iter++;
			int isBot = (int)*isBotPtr;
			player->SetProperty("bot", va("%d", isBot));
			if (isBot)
				m_numBots++;
		}

		m_players.Append(player);
	}

	// skip osmask
	iter += 4;
	iter++;	// skip sth else unknown

	if (iter < end)
	{
		int ranked = *iter;
		m_serverInfo.Set("ranked", ranked ? "Ranked" : "Unranked");
	}

	// time left
	if (iter + 4 < end)
	{
//		m_serverInfo.SetInt("timeleft", *((int*)iter));
		iter += 4;
	}

	if (iter < end)
	{
//		m_serverInfo.SetInt("gamestate", *iter);
		iter++;
	}

	if (iter + 9 < end)
	{
		m_serverInfo.SetBool("si_tv", *iter);
		iter++;
		iter++; // numinterested

		int numViewers = (*((short*)iter));
//		numViewers = htonl(numViewers);
		iter += 4;
		m_serverInfo.SetInt("ri_numviewers", numViewers);

		int maxViewers = (*((short*)iter));
//		maxViewers = htonl(maxViewers);
		iter += 4;
		m_serverInfo.SetInt("ri_maxviewers", maxViewers);
	}

	if (m_serverInfo.GetBool("si_tv"))
		m_serverInfo.Set("players", va("%d / %d", m_serverInfo.GetInt("ri_numviewers"), m_serverInfo.GetInt("ri_maxviewers")));
	else if (m_numBots > 0)
		m_serverInfo.Set("players", va("%d+%d / %d", m_players.Num() - m_numBots, m_numBots, m_serverInfo.GetInt("si_maxplayers")));
	else
		m_serverInfo.Set("players", va("%d / %d", m_players.Num(), m_serverInfo.GetInt("si_maxplayers")));


	free(packet);

	return true;
}

char *ETQWServer::ParsePlayerData(char *pos, char *packetEnd, PlayerDetails *player)
{

	return pos;
}

const char *ETQWServer::GetGametype() const
{
	return m_serverInfo.GetString("si_rules");
}
