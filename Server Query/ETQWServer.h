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

#include "doom3server.h"
#include "SupportedGame.h"

class ETQWServer : public Doom3Server
{
public:
	ETQWServer(const idStr& host, const unsigned short port);
	~ETQWServer(void);

	virtual const char* GetBaseGame() const { return "base"; }
	virtual const char* GetGametype() const;

	virtual void TranslateServerinfo(const idStr key, idStr& value);
	virtual bool ParseResponse(void* rawPacket, int packetlen);
	char* ParsePlayerData(char* pos, char* packetEnd, PlayerDetails* player);

	virtual const char* GetConnectString() const { return "+set net_autoConnectServer"; }
};
