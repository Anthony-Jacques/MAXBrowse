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

#include "MasterServer.h"

enum ListColumnType;

#define	DEFAULT_GAME_VERSION	"<default>"

#define MAX_COLUMNS		50

class ColumnDetails
{
public:
	ColumnDetails(idStr var, idStr n, ListColumnType s, bool h, int o, int w)
		: variable(var), name(n), sortType(s), hidden(h), order(o), width(w)
	{
	}

	idStr variable;
	idStr name;
	ListColumnType sortType;
	bool hidden;
	int order;
	int width;
};


class SupportedGame
{
public:
	SupportedGame(MSXML2::IXMLDOMNodePtr xmlNode);
	~SupportedGame(void);

	const idStr &GetName() const			{ return m_name; }
	MasterServer *GetMasterServer() const	{ return m_masterServer; }

	bool GetForceNoSMP() const				{ return m_forceNoSMP; }
	bool IsInstalled() const				{ return m_isInstalled; }
	int GetSortColumn() const				{ return m_sortColumn; }
	bool GetReverseSort() const				{ return m_reverseSort; }
	idStr GetProtocolFilter() const			{ return m_protocolFilter; }
	idStr GetFavouritesFilter() const		{ return m_favouritesFilter; }
	idStr GetCurrentFilterPos()	const		{ return m_currentFilterPos; }
	unsigned short GetDefaultGamePort() const { return m_defaultGamePort; }

	const char *GetExeName(const char *version) const					{ return m_exeName.GetString(version); }
	const char *GetInstallDir(const char *version) const				{ return m_installDir.GetString(version); }
	const char *GetAdditionalParams(const char *version) const			{ return m_additionalParams.GetString(version);; }

	const idDict &GetExeNameDict() {  return m_exeName; }
	const idDict &GetInstallDirDict() { return m_installDir; }
	const idDict &GetAdditionalParamDict() { return m_additionalParams; }

	void SetExeNames(idDict newVals)			{ m_exeName = newVals; }
	void SetInstallDirs(idDict newVals)			{ m_installDir = newVals; }
	void SetAdditionalParams(idDict newVals)	{ m_additionalParams = newVals; }

	void SetForceNoSMP(bool newVal)			{ m_forceNoSMP = newVal; }
	void SetInstalled(bool newVal)			{ m_isInstalled = newVal; }
	void SetSortColumn(int col)				{ m_sortColumn = col; }
	void SetReverseSort(bool rev)			{ m_reverseSort = rev; }
	void SetProtocolFilter(idStr newVal)	{ m_protocolFilter = newVal; }
	void SetFavouritesFilter(idStr newVal)	{ m_favouritesFilter = newVal; }
	void SetCurrentFilterPos(idStr newVal)	{ m_currentFilterPos = newVal; }

	idList<idStr*> &GetFilterStructure()	{ return m_filterStructure; }
	idList<ColumnDetails*> &GetColumnDetails() { return columns; }
	idList<idStr> &GetPlayerDetails() { return m_playerProperties; }

	const char *GetPunkbusterCvar();
	const char *GetPasswordedCvar();
	const char *GetMaxPlayersCvar();

	void LoadConfigurationSettings();
	void SaveConfigurationSettings();

	bool Connect(const GameServer *gameServer) const;
protected:
	void LoadColumns();
	void SaveColumns();

	void SaveFilterTree();
	void LoadFilterTree();

	void LoadInstallations();
	void SaveInstallations();
	void EnumDictRegKeys(idDict *theDict, char *keyName);

	void AddDefaultFilter();

	idStr m_name;
	bool m_isInstalled;
	bool m_forceNoSMP;
	unsigned short m_defaultGamePort;

	idDict m_exeName;
	idDict m_installDir;
	idDict m_additionalParams;

	// FIXME: current assumption is 1 master per game. In the case of some games this 
	// isnt the case, so this may need reviewing at some stage.
	MasterServer *m_masterServer;	

	idList<idStr*> m_defaultFilter;
	idList<idStr*> m_filterStructure;
	idList<ColumnDetails*> columns;
	idList<idStr> m_playerProperties;

	int m_sortColumn;
	bool m_reverseSort;

	idStr m_protocolFilter;
	idStr m_favouritesFilter;
	idStr m_currentFilterPos;

	idStr m_punkbusterCvar;
	idStr m_passwordCvar;
	idStr m_maxplayersCvar;

};
