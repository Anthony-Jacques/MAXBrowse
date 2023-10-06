#pragma once
#include "Doom3Master.h"

class PreyMaster : public Doom3Master
{
public:

	PreyMaster(const idStr &host = "209.131.124.233", const unsigned short port = 27655)
		: Doom3Master(host, port) {};

	virtual ~PreyMaster() {}

	virtual const char *GetServersFilename() { return "PreyServers.xml"; }
};
