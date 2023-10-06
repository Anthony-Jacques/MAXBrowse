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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GameServer.h"

#include "Doom3Master.h"
#include "Quake4Master.h"
#include "Quake3Master.h"
#include "HTTPMaster.h"

// fixme: if this ever gets pulled into Q4, need to replace with sth using idFile
#include <fstream>
using namespace std;


MasterServer *MasterServer::Construct(const idStr &protocol, const char *server, const unsigned short port, const idStr &protocolVersion)
{
	if (protocol.ICmp("Doom3Master") == 0)
		return new Doom3Master(server, port);
	else if (protocol.ICmp("Quake4Master") == 0)
		return new Quake4Master(server, port);
	else if (protocol.ICmp("Quake3Master") == 0)
		return new Quake3Master(server, port, protocolVersion);
	else if (protocol.ICmp("HTTPMaster") == 0)
		return new HTTPMaster(server, port);
	else
		return NULL;
}

MasterServer::MasterServer(const idStr &host, const unsigned short port)
	: SocketConnection(),
	m_host(host), m_port(port)
{
	m_loadedServerMap.InitHashTable(1337);
}

MasterServer::~MasterServer(void)
{
	m_newServers.Clear();
	m_servers.DeleteContents(true);
}

void MasterServer::ClearNewServersList()
{
	m_newServers.Clear();
}

void MasterServer::DeleteOldServers()
{
	for (int i=0; i<m_servers.Num(); i++)
	{
		if (m_servers[i]->GetPing() == PING_TIMEOUT && m_servers[i]->GetFavourites()->GetNumKeyVals() == 0)
		{
			m_loadedServerMap.RemoveKey(va("%s:%d", m_servers[i]->GetHost().c_str(), m_servers[i]->GetPort()));

			delete m_servers[i];
			m_servers.RemoveIndex(i);
			i--;
		}
	}
}

void MasterServer::DeleteServer(GameServer *theServer)
{
	m_loadedServerMap.RemoveKey(va("%s:%d", theServer->GetHost().c_str(), theServer->GetPort()));

	for (int i=m_servers.Num() - 1; i >= 0; i--)
	{
		if (m_servers[i] == theServer)
		{
			m_servers.RemoveIndex(i);
			break;
		}
	}

	delete theServer;
}

bool MasterServer::LoadServers()
{
	CFileFind finder;
	if (!finder.FindFile(m_ServersFilename.c_str()))
		return false;

	CoInitialize(NULL);

	bool success = true;

	MSXML2::IXMLDOMDocument2Ptr pXMLDom;
	HRESULT hr = pXMLDom.CreateInstance(__uuidof(DOMDocument));
	if (FAILED(hr)) 
	{
		AfxMessageBox("Failed to CreateInstance of an XML DOM.\nCheck msxml2 has been registered", MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
		success = false;
	}

	if (success)
	{
		// Set parser property settings
		pXMLDom->async = VARIANT_FALSE;
		
		// Load the XML file
		hr = pXMLDom->load(m_ServersFilename.c_str());
		
		// If document does not load report the parse error 
		if (hr != VARIANT_TRUE)
		{
			ParseError(pXMLDom);
			success = false;
		}
	}

	if (success)
	{
		// Set the selection namespace.
		pXMLDom->setProperty("SelectionLanguage", "XPath");

		LoadXML(pXMLDom->documentElement);
	}

	if (pXMLDom)
		pXMLDom.Release();

	CoUninitialize();

	return success;
}


void MasterServer::SaveServers()
{
	idStr filename = m_ServersFilename;

	ofstream file;
	file.open(filename.c_str(), ios::out);
	if (file.is_open())
	{
		idStr xmlHeaderStr = va("<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?>\n" \
			"<masterserver host=\"%s\" port=\"%d\">\n", m_host.c_str(), m_port);

		file.write(xmlHeaderStr.c_str(), xmlHeaderStr.Length());

		int numServers = m_servers.Num();
		for (int i=0; i<numServers; i++)
		{
			idStr serverStr = "";
			m_servers[i]->StorePropertiesXML(serverStr);

			file.write(serverStr.c_str(), serverStr.Length());
			file.write("\n", 1);
		}

		idStr xmlFooterStr = "</masterserver>\n";
		file.write(xmlFooterStr.c_str(), xmlFooterStr.Length());

		file.close();
	}
}

void MasterServer::ParseError(MSXML2::IXMLDOMDocument2Ptr pXMLDom)
{
	// Try to output a meaningful error message.
	MSXML2::IXMLDOMParseErrorPtr  pError;
	pError = pXMLDom->parseError;

	long line = pError->Getline();
	long pos = pError->Getlinepos();

	if ((line == 0) && (pos == 0))
	{
		// Probably failed to open the file.
		_bstr_t parseError =
			_bstr_t("Failed at the start of the file.\nDoes the file exist?\n\n") +
			_bstr_t("File: ") + _bstr_t(pError->Geturl()) + _bstr_t("\n\n") +
			_bstr_t(pError->Getreason());
		MessageBox(NULL, parseError, "Parse Error", MB_OK);
	}
	else
	{
		// General error.
		_bstr_t parseError =
			_bstr_t("File: ") + _bstr_t(pError->Geturl()) + _bstr_t("\n\n") +
			_bstr_t("Line: ") + _bstr_t(pError->Getline()) + _bstr_t("\n") +
			_bstr_t("Pos:  ") + _bstr_t(pError->Getlinepos()) + _bstr_t("\n\n") +
			_bstr_t(pError->Getreason());
		MessageBox(NULL, parseError, "Parse Error", MB_OK);
	}
}

void MasterServer::LoadXML(MSXML2::IXMLDOMElementPtr pMasterServerNode)
{
	MSXML2::IXMLDOMNodeListPtr pXMLDomNodeList = pMasterServerNode->selectNodes(".//server");

	int count = pXMLDomNodeList->length;

	for (int i = 0; i < count; i++)
	{
		MSXML2::IXMLDOMNodePtr pServerNode = pXMLDomNodeList->Getitem(i);

		idStr hostname = XMLGetAttribute(pServerNode, "host")->GetnodeValue();
		idStr port = XMLGetAttribute(pServerNode, "port")->GetnodeValue();

		int x = 1;
		GameServer *thisServer = AddServer(hostname.c_str(), atoi(port.c_str()));
		thisServer->LoadXML(pServerNode);
	}
}

GameServer *MasterServer::AddServer(const char *hostname, unsigned short port)
{
	GameServer *thisServer = GameServer::Construct(m_serverProtocol, hostname, port);

	return MasterServer::AddServer(thisServer);
}

GameServer *MasterServer::AddServer(GameServer *thisServer)
{
	m_loadedServerMap[va("%s:%d", thisServer->GetHost().c_str(), thisServer->GetPort())] = thisServer;
	int count = m_loadedServerMap.GetCount();

	m_servers.Append(thisServer);

	return thisServer;
}

bool MasterServer::IsNewServer(const char *hostname, unsigned short port)
{
	GameServer *thisServer = (GameServer *)m_loadedServerMap[va("%s:%d", hostname, port)];

	return (thisServer == NULL);
}

GameServer *MasterServer::GetServer(const char *hostname, unsigned short port)
{
	GameServer *thisServer = (GameServer *)m_loadedServerMap[va("%s:%d", hostname, port)];

	return thisServer;
}

int MasterServer::ImportIPs(CString ipList)
{
	int count = 0;

	int curPos = 0;
	CString hostName = ipList.Tokenize(":", curPos);
	while (hostName != "")
	{
		CString portStr = ipList.Tokenize("\n", curPos);
		short port = atoi(portStr);

		if (IsNewServer(hostName, port))
		{
			GameServer *thisServer = AddServer(hostName, port);
			m_newServers.Append(thisServer);
			count++;
		}

		hostName = ipList.Tokenize(":", curPos);
	}

	return count;
}

