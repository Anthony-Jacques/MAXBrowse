#pragma once
#include "Doom3Master.h"

class ETQWMaster : public Doom3Master
{
public:

	ETQWMaster(const idStr &host = "127.0.0.1", const unsigned short port = 27655);

	virtual ~ETQWMaster();

	virtual const char *GetServersFilename() { return "ETQWServers.xml"; }

	const char *GetQueryString(int filter, int &len);
	GameServer *ETQWMaster::AddServer(const char *hostname, unsigned short port);

};
