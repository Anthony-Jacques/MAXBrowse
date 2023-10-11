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
#include ".\supportedgame.h"

#include "SortedListView.h"

#include "ServerBrowser.h"
#include "GameServer.h"

#include "MasterServer.h"

#define STD_REG_KEY(x) (( HKEY ) (ULONG_PTR)((LONG)0x80000000 | x) )

SupportedGame::SupportedGame(MSXML2::IXMLDOMNodePtr xmlNode)
{
	CComPtr<MSXML2::IXMLDOMNode> attrib = XMLGetAttribute(xmlNode, "name");
	m_name = attrib ? attrib->GetnodeValue() : "";

	// query settings
	CComPtr<MSXML2::IXMLDOMNode> querySettings = xmlNode->selectSingleNode(".//query");
	attrib = XMLGetAttribute(querySettings, "masterProtocol");
	idStr masterProtocol = attrib ? attrib->GetnodeValue() : "";

	attrib = XMLGetAttribute(querySettings, "masterServer");
	idStr masterServer = attrib ? attrib->GetnodeValue() : "";

	attrib = XMLGetAttribute(querySettings, "masterPort");
	idStr masterPort = attrib ? attrib->GetnodeValue() : "";

	attrib = XMLGetAttribute(querySettings, "protocolVersion");
	idStr protocolVersion = attrib ? attrib->GetnodeValue() : "";

	m_masterServer = MasterServer::Construct(masterProtocol, masterServer.c_str(), atoi(masterPort.c_str()), protocolVersion);

	attrib = XMLGetAttribute(querySettings, "gamePort");
	idStr gamePort = attrib ? attrib->GetnodeValue() : "";
	m_defaultGamePort = atoi(gamePort.c_str());

	attrib = XMLGetAttribute(querySettings, "serverProtocol");
	idStr serverProtocol = attrib ? attrib->GetnodeValue() : "";
	m_masterServer->SetServerProtocol(serverProtocol);

	attrib = XMLGetAttribute(xmlNode, "serversFile");
	m_masterServer->SetServersFilename(attrib ? attrib->GetnodeValue() : "");

	CComPtr<MSXML2::IXMLDOMNode> installSettings = xmlNode->selectSingleNode(".//installSettings");

	// Install Settings
	attrib = XMLGetAttribute(installSettings, "defaultExe");
	idStr lDefaultExe = attrib ? attrib->GetnodeValue() : "";

	attrib = XMLGetAttribute(installSettings, "defaultPath");
	idStr lDefaultPath = attrib ? attrib->GetnodeValue() : "";
	
	attrib = XMLGetAttribute(installSettings, "installRegistrySection");
	idStr lInstallRegistrySection = attrib ? attrib->GetnodeValue() : "";

	attrib = XMLGetAttribute(installSettings, "installRegistryKey");
	idStr lInstallRegistryKey = attrib ? attrib->GetnodeValue() : "";

	attrib = XMLGetAttribute(installSettings, "installRegistryValue");
	idStr lInstallRegistryValue = attrib ? attrib->GetnodeValue() : "";
	CString installRegKey;

	m_exeName.Set(DEFAULT_GAME_VERSION, lDefaultExe.c_str());
	m_isInstalled = GetRegistryString(installRegKey, STD_REG_KEY(atoi(lInstallRegistrySection.c_str())), lInstallRegistryKey.c_str(), lInstallRegistryValue.c_str(), lDefaultPath.c_str());
// fixme: strip path from installRegKey - for wsw
	m_installDir.Set(DEFAULT_GAME_VERSION, (const char*)installRegKey);
	m_additionalParams.Set(DEFAULT_GAME_VERSION, "");

	// special cvars
	CComPtr<MSXML2::IXMLDOMNode> specialCvars = xmlNode->selectSingleNode(".//specialCvars");
	attrib = XMLGetAttribute(specialCvars, "pbcvar");
	m_punkbusterCvar = attrib ? attrib->GetnodeValue() : "";

	attrib = XMLGetAttribute(specialCvars, "pwcvar");
	m_passwordCvar = attrib ? attrib->GetnodeValue() : "";

	attrib = XMLGetAttribute(specialCvars, "maxplayerscvar");
	m_maxplayersCvar = attrib ? attrib->GetnodeValue() : "";

	// column details
	MSXML2::IXMLDOMNodeListPtr pXMLDomNodeList = xmlNode->selectNodes(".//columns/column");
	int count = pXMLDomNodeList->length;

	for (int i = 0; i < count; i++)
	{
		MSXML2::IXMLDOMNodePtr pColumnNode = pXMLDomNodeList->Getitem(i);

		idStr name = XMLGetAttribute(pColumnNode, "name")->GetnodeValue();
		idStr cvar = XMLGetAttribute(pColumnNode, "cvar")->GetnodeValue();
		idStr colType = XMLGetAttribute(pColumnNode, "type")->GetnodeValue();
		attrib = XMLGetAttribute(pColumnNode, "defaultVisible");
		idStr defaultHidden = attrib ? attrib->GetnodeValue() : "0";

		columns.Append(new ColumnDetails(cvar.c_str(), name.c_str(), (ListColumnType)atoi(colType.c_str()), atoi(defaultHidden.c_str()), -1, 50));
	}

	m_sortColumn = 0;
	m_reverseSort = false;

	// player columns
	pXMLDomNodeList = xmlNode->selectNodes(".//playerProperties/property");
	count = pXMLDomNodeList->length;

	for (int i = 0; i < count; i++)
	{
		MSXML2::IXMLDOMNodePtr pColumnNode = pXMLDomNodeList->Getitem(i);

		idStr name = XMLGetAttribute(pColumnNode, "name")->GetnodeValue();
		m_playerProperties.Append(name);
	}

	// default filter
	pXMLDomNodeList = xmlNode->selectNodes(".//defaultFilter/property");
	count = pXMLDomNodeList->length;

	for (int i = 0; i < count; i++)
	{
		MSXML2::IXMLDOMNodePtr pColumnNode = pXMLDomNodeList->Getitem(i);
		idStr name = XMLGetAttribute(pColumnNode, "name")->GetnodeValue();
		m_defaultFilter.Append(new idStr(name));
	}

	// m_filterStructure
	m_forceNoSMP = false;	// fixme

	m_protocolFilter = "";
	m_favouritesFilter = "";

	m_currentFilterPos = m_name;
}



SupportedGame::~SupportedGame(void)
{
	SaveConfigurationSettings();

	if (m_masterServer)
		delete m_masterServer;

	columns.DeleteContents(true);
	m_filterStructure.DeleteContents(true);
}


void SupportedGame::LoadConfigurationSettings()
{
	m_isInstalled = theApp.GetProfileInt(va("Settings\\Games\\%s", m_name.c_str()), "IsInstalled", m_isInstalled);

	m_protocolFilter = (const char*)theApp.GetProfileString(va("Settings\\Games\\%s", m_name.c_str()), "ProtocolFilter", m_protocolFilter.c_str());
	m_favouritesFilter = (const char*)theApp.GetProfileString(va("Settings\\Games\\%s", m_name.c_str()), "FavouritesFilter", m_favouritesFilter.c_str());
	m_currentFilterPos = (const char*)theApp.GetProfileString(va("Settings\\Games\\%s", m_name.c_str()), "CurrentFilterPos", m_currentFilterPos.c_str());
	m_forceNoSMP = theApp.GetProfileInt(va("Settings\\Games\\%s", m_name.c_str()), "ForceNoSMP", m_forceNoSMP);

	LoadInstallations();
	LoadColumns();
	LoadFilterTree();
}

void SupportedGame::LoadColumns()
{
	int numColumns = columns.Num();
	int numVisibleColumns = 0;
	for (int i=0; i<numColumns; i++)
	{
		ColumnDetails *thisColumn = columns[i];
		thisColumn->hidden = theApp.GetProfileInt(va("Settings\\Games\\%s\\Columns\\Hidden", m_name.c_str()), thisColumn->variable.c_str(), thisColumn->hidden);
		thisColumn->width = theApp.GetProfileInt(va("Settings\\Games\\%s\\Columns\\Widths", m_name.c_str()), thisColumn->variable.c_str(), thisColumn->width);
		thisColumn->order = theApp.GetProfileInt(va("Settings\\Games\\%s\\Columns\\Order", m_name.c_str()), thisColumn->variable.c_str(), thisColumn->order);
	}

	m_sortColumn = theApp.GetProfileInt(va("Settings\\Games\\%s\\Columns", m_name.c_str()), "SortColumn", m_sortColumn);
	m_reverseSort = theApp.GetProfileInt(va("Settings\\Games\\%s\\Columns", m_name.c_str()), "ReverseSort", m_reverseSort);
}

void SupportedGame::LoadFilterTree()
{
	m_filterStructure.DeleteContents(true);

	idStr parentKey = va("Settings\\Games\\%s\\Filter", m_name.c_str());

	int numItems = theApp.GetProfileInt(parentKey.c_str(), "Depth", 0);

	if (numItems == 0)
		AddDefaultFilter();

	for (int i=0; i<numItems; i++)
	{
		idStr keyName = va("%d", i);
		CString value = theApp.GetProfileString(parentKey.c_str(), keyName.c_str());

		idStr *valueStr = new idStr(value);

		m_filterStructure.Append(valueStr);
	}
}
void SupportedGame::SaveInstallations()
{
	RegDeleteKey(HKEY_CURRENT_USER, va("SOFTWARE\\Q4Max\\MaxBrowse\\Settings\\Games\\%s\\ExeName", m_name.c_str()));
	RegDeleteKey(HKEY_CURRENT_USER, va("SOFTWARE\\Q4Max\\MaxBrowse\\Settings\\Games\\%s\\InstallDir", m_name.c_str()));
	RegDeleteKey(HKEY_CURRENT_USER, va("SOFTWARE\\Q4Max\\MaxBrowse\\Settings\\Games\\%s\\AdditionalParams", m_name.c_str()));

	theApp.WriteProfileString(va("Settings\\Games\\%s", m_name.c_str()), "ExeName", m_exeName.GetString(DEFAULT_GAME_VERSION));
	theApp.WriteProfileString(va("Settings\\Games\\%s", m_name.c_str()), "InstallDir", m_installDir.GetString(DEFAULT_GAME_VERSION));
	theApp.WriteProfileString(va("Settings\\Games\\%s", m_name.c_str()), "AdditionalParams", m_additionalParams.GetString(DEFAULT_GAME_VERSION));

	int numInstalls = m_exeName.GetNumKeyVals();
	for (int i=0; i<numInstalls; i++)
	{
		const idKeyValue *keyVal = m_exeName.GetKeyVal(i);
		if (idStr::ICmp(DEFAULT_GAME_VERSION, keyVal->GetKey().c_str()) != 0)
		{
			theApp.WriteProfileString(va("Settings\\Games\\%s\\ExeName", m_name.c_str()), 
										keyVal->GetKey().c_str(), 
										keyVal->GetValue().c_str());
		}

		keyVal = m_installDir.GetKeyVal(i);
		if (idStr::ICmp(DEFAULT_GAME_VERSION, keyVal->GetKey().c_str()) != 0)
		{
			theApp.WriteProfileString(va("Settings\\Games\\%s\\InstallDir", m_name.c_str()), 
										keyVal->GetKey().c_str(), 
										keyVal->GetValue().c_str());
		}

		keyVal = m_additionalParams.GetKeyVal(i);
		if (idStr::ICmp(DEFAULT_GAME_VERSION, keyVal->GetKey().c_str()) != 0)
		{
			theApp.WriteProfileString(va("Settings\\Games\\%s\\AdditionalParams", m_name.c_str()), 
										keyVal->GetKey().c_str(), 
										keyVal->GetValue().c_str());
		}			
	}

}

void SupportedGame::LoadInstallations()
{
	m_exeName.Set(DEFAULT_GAME_VERSION, 
				(const char*)theApp.GetProfileString(va("Settings\\Games\\%s", m_name.c_str()), 
				"ExeName", 
				m_exeName.GetString(DEFAULT_GAME_VERSION)));

	m_installDir.Set(DEFAULT_GAME_VERSION, 
				(const char*)theApp.GetProfileString(va("Settings\\Games\\%s", m_name.c_str()), 
				"InstallDir", 
				m_installDir.GetString(DEFAULT_GAME_VERSION)));

	m_additionalParams.Set(DEFAULT_GAME_VERSION, 
				(const char*)theApp.GetProfileString(va("Settings\\Games\\%s", m_name.c_str()), 
				"AdditionalParams", 
				m_additionalParams.GetString(DEFAULT_GAME_VERSION)));

	EnumDictRegKeys(&m_exeName, "ExeName");
	EnumDictRegKeys(&m_installDir, "InstallDir");
	EnumDictRegKeys(&m_additionalParams, "AdditionalParams");
}

void SupportedGame::EnumDictRegKeys(idDict *theDict, char *keyName)
{
	HKEY hKey;
	DWORD retVal = RegOpenKeyEx(HKEY_CURRENT_USER, va("SOFTWARE\\Q4Max\\MaxBrowse\\Settings\\Games\\%s\\%s", m_name.c_str(), keyName), 0, KEY_QUERY_VALUE, &hKey);

	if (retVal == ERROR_SUCCESS)
	{
		bool done = false;
		for (int i=0; !done; i++)
		{
			char name[MAX_PATH];
			DWORD nameLen = MAX_PATH;
			BYTE value[MAX_PATH];
			DWORD valueLen = MAX_PATH;
			FILETIME lastWrite;
			DWORD regType;

			done = (RegEnumValue(hKey, i, name, &nameLen, NULL, &regType, value, &valueLen) == ERROR_NO_MORE_ITEMS);

			if (!done)
			{
				CString keyName = name;
				CString theValue = (char*)value;

				theDict->Set(name, (char*)value);
			}
		}

		RegCloseKey(hKey);
	}
}

void SupportedGame::SaveConfigurationSettings()
{
	theApp.WriteProfileInt(va("Settings\\Games\\%s", m_name.c_str()), "IsInstalled", m_isInstalled);

	if (m_isInstalled)
	{
		theApp.WriteProfileString(va("Settings\\Games\\%s", m_name.c_str()), "ProtocolFilter", m_protocolFilter.c_str());
		theApp.WriteProfileString(va("Settings\\Games\\%s", m_name.c_str()), "FavouritesFilter", m_favouritesFilter.c_str());
		theApp.WriteProfileString(va("Settings\\Games\\%s", m_name.c_str()), "CurrentFilterPos", m_currentFilterPos.c_str());
		theApp.WriteProfileInt(va("Settings\\Games\\%s", m_name.c_str()), "ForceNoSMP", m_forceNoSMP);
		SaveInstallations();
		SaveColumns();
		SaveFilterTree();
	}
}


void SupportedGame::SaveColumns()
{
	int numColumns = columns.Num();
	int numVisibleColumns = 0;
	for (int i=0; i<numColumns; i++)
	{
		ColumnDetails *thisColumn = columns[i];
		theApp.WriteProfileInt(va("Settings\\Games\\%s\\Columns\\Hidden", m_name.c_str()), thisColumn->variable.c_str(), thisColumn->hidden);
		theApp.WriteProfileInt(va("Settings\\Games\\%s\\Columns\\Widths", m_name.c_str()), thisColumn->variable.c_str(), thisColumn->width);
		theApp.WriteProfileInt(va("Settings\\Games\\%s\\Columns\\Order", m_name.c_str()), thisColumn->variable.c_str(), thisColumn->order);
	}

	theApp.WriteProfileInt(va("Settings\\Games\\%s\\Columns", m_name.c_str()), "SortColumn", m_sortColumn);
	theApp.WriteProfileInt(va("Settings\\Games\\%s\\Columns", m_name.c_str()), "ReverseSort", m_reverseSort);
}


void SupportedGame::SaveFilterTree()
{
	int numItems = m_filterStructure.Num();

	idStr parentKey = va("Settings\\Games\\%s\\Filter", m_name.c_str());
	theApp.WriteProfileInt(parentKey.c_str(), "Depth", numItems);

	for (int i=0; i<numItems; i++)
	{
		idStr keyName = va("%d", i);
		theApp.WriteProfileString(parentKey.c_str(), keyName.c_str(), m_filterStructure[i]->c_str());
	}
}


bool SupportedGame::Connect(const GameServer *gameServer) const
{
	idStr gameName = gameServer->GetServerInfo().GetString("fs_game", ""); // q4-style
	if (gameName.empty())
	{
		// try q3 style
		gameName = gameServer->GetServerInfo().GetString("gamename", gameServer->GetBaseGame());
	}

	if (gameName.ICmp(gameServer->GetBaseGame()) != 0)
		gameName = va("+set fs_game %s", gameName.c_str());

	PROCESS_INFORMATION processInfo;
	memset(&processInfo, 0, sizeof(processInfo));
	STARTUPINFO startupInfo;
	memset(&startupInfo, 0, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);

	const char *exeName = m_exeName.GetString(gameServer->GetVersion(), m_exeName.GetString(DEFAULT_GAME_VERSION));
	const char *installDir = m_installDir.GetString(gameServer->GetVersion(), m_installDir.GetString(DEFAULT_GAME_VERSION));
	const char *additionalParms = m_additionalParams.GetString(gameServer->GetVersion(), m_additionalParams.GetString(DEFAULT_GAME_VERSION));

	const char *connectStr = gameServer->GetConnectString();
	idStr passwordStr = gameServer->GetPassword().Length() > 0 ? va("+set password \"%s\"", gameServer->GetPassword().c_str()) : "";
	idStr filePath = va("%s\\%s", installDir, exeName);
	idStr commandLine = va("%s %s %s %s %s:%d %s", 
						exeName, gameName.c_str(), passwordStr.c_str(), connectStr,
						gameServer->GetHost().c_str(), gameServer->GetPort(), 
						additionalParms);

	if (CreateProcess(filePath.c_str(), (char*)commandLine.c_str(),
					NULL, NULL,					// security stuff
					FALSE, CREATE_NEW_CONSOLE,	// misc flags
					NULL,						// environment
					installDir,					// Working directory
					&startupInfo, &processInfo) == 0)
	{
		AfxMessageBox(va("Failed to launch %s\nCheck the game configuration settings.", filePath.c_str()));
		return false;
	}
	else if (m_forceNoSMP)
	{
		DWORD dw, dwAffinity;
		GetProcessAffinityMask(processInfo.hProcess, &dwAffinity, &dw);
		dwAffinity &= 1;
		SetProcessAffinityMask(processInfo.hProcess, dwAffinity);
	}

	return true;
}


const char *SupportedGame::GetPunkbusterCvar()
{
	return m_punkbusterCvar.c_str();
}

const char *SupportedGame::GetPasswordedCvar()
{
	return m_passwordCvar.c_str();
}

const char *SupportedGame::GetMaxPlayersCvar()
{
	return m_maxplayersCvar.c_str();
}

void SupportedGame::AddDefaultFilter()
{
	for (int i = 0; i < m_defaultFilter.size(); i++)
	{
		m_filterStructure.Append(new idStr(m_defaultFilter[i]));
	}
	/*
	switch (m_id)
	{
	case GAME_ETQW:
		m_filterStructure.Append(new idStr("ranked"));
		m_filterStructure.Append(new idStr("fs_game"));
		m_filterStructure.Append(new idStr("si_rules"));
		m_filterStructure.Append(new idStr("si_map"));
		break;
	case GAME_QUAKE4:
	case GAME_DOOM3:
	case GAME_PREY:
		m_filterStructure.Append(new idStr("fs_game"));
		m_filterStructure.Append(new idStr("gamename"));
		m_filterStructure.Append(new idStr("si_gametype"));
		m_filterStructure.Append(new idStr("si_map"));
		break;
	case GAME_ET:
	case GAME_QUAKE3:
		m_filterStructure.Append(new idStr("gamename"));
		m_filterStructure.Append(new idStr("g_gametype"));
		m_filterStructure.Append(new idStr("mapname"));
		break;
	case GAME_WARSOW:
		m_filterStructure.Append(new idStr("g_gametype"));
		m_filterStructure.Append(new idStr("mapname"));
		break;
	default:
		m_filterStructure.Append(new idStr("game"));
		m_filterStructure.Append(new idStr("gamename"));
		m_filterStructure.Append(new idStr("mapname"));
		break;
	}
	*/
}
