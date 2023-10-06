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

#pragma once
#include "Quake3Server.h"

class WarsowServer : public Quake3Server
{
public:
	WarsowServer(const idStr &host, const unsigned short port)
		: Quake3Server(host, port)
	{
	}

	virtual const char *GetBaseGameCvar() const { return "fs_game"; }
	virtual const char* GetBaseGame() const { return "basewsw"; }
	virtual const char* GetMod() const;

	virtual const char *GetQueryString(int &len);
	virtual bool IsValidPacket(void *ptr);
	virtual char *GetPacketData(void *ptr);
	virtual void ParsePlayerData(idStr str, PlayerDetails *player);

	virtual bool IsTV() const;
	virtual void TranslateGameType() {}
};
