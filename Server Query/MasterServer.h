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

#include "SocketConnection.h"

class GameServer;


class MasterServer : public SocketConnection
{
public:
	static MasterServer *Construct(const idStr &protocol, const char *server, const unsigned short port, const idStr &protocolVersion);

	virtual ~MasterServer(void);

	virtual bool Query(int filter) = 0;

	const idList<GameServer*>&GetServers() const { return m_servers; }
	const idList<GameServer*>&GetNewServers() const { return m_newServers; }

	void ClearNewServersList();

	bool LoadServers();
	void SaveServers();

	int ImportIPs(CString ipList);

	void DeleteOldServers();
	void DeleteServer(GameServer *theServer);

	void ParseError(MSXML2::IXMLDOMDocument2Ptr pXMLDom);
	void LoadXML(MSXML2::IXMLDOMElementPtr pCINode);

	virtual GameServer *AddServer(const char *hostname, unsigned short port);
	GameServer *GetServer(const char *hostname, unsigned short port);

	void SetServersFilename(idStr serversFilename) { m_ServersFilename = serversFilename; }
	void SetServerProtocol(idStr serverProtocol) { m_serverProtocol = serverProtocol; }
protected:
	MasterServer(const idStr &host, unsigned short port);
	virtual GameServer *AddServer(GameServer *thisServer);

	bool IsNewServer(const char *hostname, unsigned short port);

	idStr m_ServersFilename;
	idStr m_serverProtocol;

	idStr m_host;
	unsigned short m_port;

	idList<GameServer*> m_servers;
	idList<GameServer*> m_newServers;

	CMapStringToPtr	m_loadedServerMap;

};
