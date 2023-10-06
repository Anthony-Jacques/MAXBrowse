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

class PlayerDetails;
enum SupportedGameID;

class GameServer : public SocketConnection
{
public:
	static GameServer *Construct(const idStr &protocol, const char *server, const unsigned short port);

	virtual ~GameServer(void);

	virtual bool Query() = 0;

	virtual const char* GetBaseGame() const = 0;
	virtual const char* GetServerName() const = 0;
	virtual const char* GetMapName() const = 0;
	virtual const char* GetGametype() const = 0;
	virtual const char* GetMod() const = 0;
	virtual bool IsTV() const = 0;

	const idDict &GetServerInfo() const { return m_serverInfo; }

	void StoreProperties(idStr &output);

	void StorePropertiesXML(idStr &output);
	void LoadXML(MSXML2::IXMLDOMElementPtr pMasterServerNode);

	const idStr &GetHost() const { return m_host; }
	unsigned short GetPort() const { return m_port; }
	int GetPing() const { return m_ping; }

	const idStr &GetPassword() const { return m_password; }
	void SetPassword(const char *newPass) { m_password = newPass; }

	bool IsFavourite(const char *groupName);
	void SetFavourite(const char *groupName, bool isFav) 
	{ 
		if (isFav)
			m_favourites.SetBool(groupName, isFav); 
		else
			m_favourites.Delete(groupName); 
	}

	const idDict *GetFavourites() const { return &m_favourites; }

	const idList<PlayerDetails*> &GetPlayers() const { return m_players; }

	const char *GetVersion() const { return m_serverInfo.GetString("protocol"); }

	int GetNumPlayers() const { return m_players.Num() - m_numBots; }
	int GetNumBots() const { return m_numBots; }
protected:
	GameServer(const idStr &host, const unsigned short port);

	idStr m_host;
	unsigned short m_port;
	idStr m_password;

	int m_ping;

	idDict m_favourites;
	idDict m_serverInfo;
	idList<PlayerDetails*> m_players;

protected:
	int m_numBots;

	virtual void TranslateServerinfo(const idStr key, idStr &value) {}
};
