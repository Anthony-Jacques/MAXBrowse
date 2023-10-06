#include "StdAfx.h"
#include "ETQWMaster.h"
#include "ETQWServer.h"


ETQWMaster::ETQWMaster(const idStr &host, const unsigned short port)
	: Doom3Master(host, port)
{
}

ETQWMaster::~ETQWMaster(void)
{
}

const char *ETQWMaster::GetQueryString(int filter, int &len)
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


GameServer *ETQWMaster::AddServer(const char *hostname, unsigned short port)
{
	GameServer *thisServer = new ETQWServer(hostname, port);

	return MasterServer::AddServer(thisServer);
}