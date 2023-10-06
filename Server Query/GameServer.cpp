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

#include "StdAfx.h"
#include "GameServer.h"
#include "PlayerDetails.h"
#include "IPNationalityMap.h"

#include "Doom3Server.h"
#include "Quake4Server.h"
#include "ETQWServer.h"
#include "WarsowServer.h"
#include "Quake2Server.h"
#include "ETServer.h"

GameServer *GameServer::Construct(const idStr &protocol, const char *server, const unsigned short port)
{
	if (protocol.ICmp("Doom3Server") == 0)
		return new Doom3Server(server, port);
	else if (protocol.ICmp("Quake4Server") == 0)
		return new Quake4Server(server, port);
	else if (protocol.ICmp("Quake3Server") == 0)
		return new Quake3Server(server, port);
	else if (protocol.ICmp("ETServer") == 0)
		return new ETServer(server, port);
	else if (protocol.ICmp("ETQWServer") == 0)
		return new ETQWServer(server, port);
	else if (protocol.ICmp("WarsowServer") == 0)
		return new WarsowServer(server, port);
	else if (protocol.ICmp("Quake2Server") == 0)
		return new Quake2Server(server, port);
	else
		return NULL;
}

GameServer::GameServer(const idStr &host, const unsigned short port)
	: SocketConnection(),
	m_host(host), m_port(port), m_ping(PING_TIMEOUT), m_password(""), m_numBots(0)
{
	m_serverInfo.SetInt("ping", m_ping);
	m_serverInfo.Set("nationality", IPNationalityMap::GetIPNationalityMap()->GetNationality(host.c_str()));
	m_serverInfo.Set("server", va("%s:%d", host.c_str(), (int)port));

	m_players.Clear();
}


GameServer::~GameServer()
{
	m_players.DeleteContents(true);

	m_serverInfo.Clear();
}



void GameServer::StoreProperties(idStr &output)
{
	output += va("\"%s\"\t%d\t%d\t", m_host.c_str(), m_port, m_ping);

	int numKeys = m_serverInfo.GetNumKeyVals();
	for (int j=0; j<numKeys; j++)
	{
		const idKeyValue *thisPair = m_serverInfo.GetKeyVal(j);
		
		idStr value = thisPair->GetValue();
		output += va("\"%s\" \"%s\"\t", thisPair->GetKey().c_str(), value.c_str());
	}

}



void GameServer::StorePropertiesXML(idStr &output)
{
	idStr passwordStr = m_password.Length() > 0 ? va(" password=\"%s\"", m_password.c_str()) : "";
	output += va("\t<server host=\"%s\" port=\"%d\" ping=\"%d\"%s>\n", 
					m_host.c_str(), m_port, m_ping, passwordStr.c_str());

	output += "\t\t<serverinfo>\n";

	int numKeys = m_serverInfo.GetNumKeyVals();
	for (int j=0; j<numKeys; j++)
	{
		const idKeyValue *thisPair = m_serverInfo.GetKeyVal(j);
		
		idStr key = XMLEscapedString(thisPair->GetKey());
		idStr value = XMLEscapedString(thisPair->GetValue());
		output += va("\t\t\t<data key=\"%s\" value=\"%s\"/>\n", key.c_str(), value.c_str());
	}

	output += "\t\t</serverinfo>\n";

	int numPlayers = m_players.Num();
	if (numPlayers > 0)
	{
		output += "\t\t<players>\n";
		for (int j=0; j<numPlayers; j++)
		{
			m_players[j]->StorePropertiesXML(output);
		}
		output += "\t\t</players>\n";
	}

	int numFavGroups = m_favourites.GetNumKeyVals();
	if (numFavGroups > 0)
	{
		output += "\t\t<favourites>\n";

		for (int j=0; j<numFavGroups; j++)
		{
			const idKeyValue *thisPair = m_favourites.GetKeyVal(j);
			
			if (idStr::ICmp(thisPair->GetValue().c_str(), "1") == 0)
			{
				idStr name = XMLEscapedString(thisPair->GetKey());
				output += va("\t\t\t<group name=\"%s\"/>\n", name.c_str());
			}
		}

		output += "\t\t</favourites>\n";
	}

	output += "\t</server>\n";
}


void GameServer::LoadXML(MSXML2::IXMLDOMElementPtr pServerNode)
{
	CComPtr<MSXML2::IXMLDOMNode> passwordAttrib = XMLGetAttribute(pServerNode, "password");
	m_password = passwordAttrib ? passwordAttrib->GetnodeValue() : "";

	/// temp compatibility thing. Remove this before public release.
	CComPtr<MSXML2::IXMLDOMNode> favouriteAttrib = XMLGetAttribute(pServerNode, "favourite");
	if (favouriteAttrib && atoi(idStr(favouriteAttrib->GetnodeValue()).c_str()))
		m_favourites.SetBool("Favourites", true);

	MSXML2::IXMLDOMNodeListPtr pXMLDomNodeList = pServerNode->selectNodes(".//serverinfo/data");
	int count = pXMLDomNodeList->length;

	for (int i = 0; i < count; i++)
	{
		MSXML2::IXMLDOMNodePtr pServerInfoNode = pXMLDomNodeList->Getitem(i);

		idStr key = XMLGetAttribute(pServerInfoNode, "key")->GetnodeValue();
		idStr value = XMLGetAttribute(pServerInfoNode, "value")->GetnodeValue();

		m_serverInfo.Set(key.c_str(), value.c_str());
	}

	pXMLDomNodeList = pServerNode->selectNodes(".//players/player");
	count = pXMLDomNodeList->length;

	m_numBots = 0;
	for (int i = 0; i < count; i++)
	{
		MSXML2::IXMLDOMNodePtr pPlayerDetailsNode = pXMLDomNodeList->Getitem(i);

		PlayerDetails *player = PlayerDetails::LoadFromXML(pPlayerDetailsNode);
		if (player)
		{
			m_players.Append(player);
			if (atoi(player->GetProperty("bot")))
				m_numBots++;
		}
	}

	pXMLDomNodeList = pServerNode->selectNodes(".//favourites/group");
	count = pXMLDomNodeList->length;

	for (int i = 0; i < count; i++)
	{
		MSXML2::IXMLDOMNodePtr pFavouriteNode = pXMLDomNodeList->Getitem(i);

		idStr group = XMLGetAttribute(pFavouriteNode, "name")->GetnodeValue();
		m_favourites.SetBool(group.c_str(), true);
	}

	m_ping = m_serverInfo.GetInt("ping", va("%d", PING_TIMEOUT));
}


bool GameServer::IsFavourite(const char *filterString)
{
	return m_favourites.GetBool(filterString);
}
