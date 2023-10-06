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
#include "GameServer.h"
#include "SupportedGame.h"

class Quake3Server : public GameServer
{
public:
	Quake3Server(const idStr &host, const unsigned short port);
	~Quake3Server(void);

	virtual bool Query();

	virtual const char* GetBaseGame() const { return "baseq3"; }
	virtual const char* GetMapName() const;
	virtual const char* GetGametype() const;
	virtual const char* GetMod() const;
	virtual const char* GetMaxClientsCVar() const { return "sv_maxclients"; }
	virtual bool IsTV() const;


	virtual const char *GetServerName() const;

	virtual void ParsePlayerData(idStr str, PlayerDetails *player);

	virtual const char *GetQueryString(int &len);

	virtual bool IsValidPacket(void *ptr);
	virtual char *GetPacketData(void *ptr);
	virtual const char *GetBaseGameCvar() const { return "game"; }

	idStr ParseString(char **data);
	void TranslateServerinfo(const idStr key, idStr &value);
	virtual void TranslateGameType();
};



