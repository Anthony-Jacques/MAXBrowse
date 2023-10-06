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

class Quake4Server : public Doom3Server
{
public:
	Quake4Server(const idStr &host, const unsigned short port);
	~Quake4Server(void);

	virtual const char* GetBaseGame() const { return "q4base"; }

	virtual char *ParsePlayerData(char *pos, char *packetEnd, PlayerDetails *player);

	virtual void TranslateServerinfo(const idStr key, idStr &value);
};
