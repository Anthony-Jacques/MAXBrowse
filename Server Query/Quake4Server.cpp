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
#include ".\quake4server.h"

#include "PlayerDetails.h"

Quake4Server::Quake4Server(const idStr &host, const unsigned short port)
	: Doom3Server(host, port)
{
}


Quake4Server::~Quake4Server(void)
{
}

char *Quake4Server::ParsePlayerData(char *pos, char *packetEnd, PlayerDetails *player)
{
	pos = Doom3Server::ParsePlayerData(pos, packetEnd, player);

	if (pos < packetEnd)
	{
		idStr clan = ParseString(&pos);
		clan.RemoveEscapes(false);

		player->SetProperty("clan", clan);
	}

	return pos;
}

void Quake4Server::TranslateServerinfo(const idStr key, idStr &value)
{
	if (key == "protocol")
	{
		if (value == "2.62")
			value = "Q4 1.0";
		else if (value == "2.63")
			value = "Q4 1.0 (German)";
		else if (value == "2.66")
			value = "Q4 Demo";
		else if (value == "2.67")
			value = "Q4 1.1 Beta";
		else if (value == "2.68")
			value = "Q4 1.1";
		else if (value == "2.69")
			value = "Q4 1.2";
		else if (value == "2.71")
			value = "Q4 1.3";
		else if (value == "2.76")
			value = "Q4 1.4 Beta";
		else if (value == "2.77")
			value = "Q4 1.4.1";
		else if (value == "2.85")
			value = "Q4 1.4.2";
		else if (value == "2.77 DE")
			value = "Q4 1.4.1 (German)";
		else if (value == "2.71 DE")
			value = "Q4 1.3 (German)";
		else if (value == "2.85 DE")
			value = "Q4 1.4.2 (German)";
		else if (value == "2.86")
			value = "Q4 1.4.2 Demo";
		else
			value = va("Protocol %s", value.c_str());
	}
}