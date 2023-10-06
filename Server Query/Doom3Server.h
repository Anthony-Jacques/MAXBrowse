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
#include "gameserver.h"
#include "SupportedGame.h"

class Doom3Server : public GameServer
{
public:
	Doom3Server(const idStr &host, const unsigned short port);
	~Doom3Server(void);

	virtual bool Query();

	virtual const char* GetBaseGame() const { return "base"; }

	virtual const char *GetServerName() const;
	virtual const char *GetMapName() const;
	virtual const char *GetGametype() const;
	virtual const char* GetMod() const;
	virtual bool IsTV() const;

	virtual char *ParsePlayerData(char *pos, char *packetEnd, PlayerDetails *player);
	virtual bool ParseResponse(void *rawPacket, int packetlen);

	char *ParseString(char **data);

	virtual void TranslateServerinfo(const idStr key, idStr &value);

};
