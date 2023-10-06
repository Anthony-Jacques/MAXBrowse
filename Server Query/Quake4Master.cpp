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
#include "quake4Master.h"
#include "Quake4Server.h"

Quake4Master::Quake4Master(const idStr &host, const unsigned short port)
	: Doom3Master(host, port)
{
}

Quake4Master::~Quake4Master(void)
{
}

const char *Quake4Master::GetQueryString(int filter, int &len)
{
	static char queryBuffer[] 
	      = "\xFF\xFF" //CONNECTIONLESS_MESSAGE_ID
		  "getServers\x00"
		  "\x00\x00\x00\x80"	// protocol
		  //"\x47\x00\x02\x00"	// protocol
		  "\x00" // fs_game
		  "\x00" // playername
		  "\x00" // clan
		  "\x00"; // pw, players, gametype

	len = sizeof(queryBuffer);

	return queryBuffer;
}
