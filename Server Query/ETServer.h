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

class ETServer : public Quake3Server
{
public:
	ETServer(const idStr &host, const unsigned short port = 27666)
		: Quake3Server(host, port)
	{
	}

	virtual const char* GetBaseGame() const { return "etmain"; }
	virtual bool IsTV() const;
	virtual void TranslateGameType() {}

};


