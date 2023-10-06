// ServerBrowserDoc.h : interface of the CServerBrowserDoc class
//
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


enum ViewUpdates
{
	VIEW_FILTER_CHANGED,
	VIEW_SELECTED_SERVER_CHANGED,
	VIEW_COLUMNS_CHANGED,
	VIEW_GAME_CHANGED,
	SERVERS_REFRESHED,
	SELECTED_SERVERS_REFRESHED,
	VIEW_FILTER_TREE_CHANGED,
};

#include "SupportedGame.h"

#include "Doom3Master.h"
#include "Quake4Master.h"

#include "IPNationalityMap.h"

class GameServer;

class CServerBrowserDoc : public CDocument
{
protected: // create from serialization only
	CServerBrowserDoc();
	DECLARE_DYNCREATE(CServerBrowserDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void PreCloseFrame(CFrameWnd* pFrame);
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CServerBrowserDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

protected:

	bool LoadSupportedGames();
	idList<SupportedGame*> m_supportedGames;
	SupportedGame* m_currentGame;

	idDict m_filter;
	idList<GameServer*> m_filteredServers;

	GameServer *m_selectedServer;

	bool m_passwordFilter;
	bool m_punkbusterFilter;
	bool m_NoPunkbusterFilter;
	bool m_emptyFilter;
	bool m_fullFilter;
	bool m_tvFilter;
	
	bool m_cancelFlag;

	bool m_useICMPPing;
	int m_queryServerCount;

	int m_serversQueried;
	idList<GameServer*> m_serversToQuery;
	CRITICAL_SECTION m_queryCritSec;

	IPNationalityMap m_nationalityMap;

	static unsigned int __stdcall ServerQueryThread(void *params);
	void QueryOutstandingServers();

	static unsigned int __stdcall MasterQueryThread(void *params);

public:
	const idList<GameServer*> &GetFilteredServers() const { return m_filteredServers; }
	void FilterServerList();
	void SetFilter(const idDict &filter) { m_filter = filter; }
	idList<idStr*> &GetFilterStructure() { return m_currentGame->GetFilterStructure(); }

	void SetSelectedServer(GameServer *server) { m_selectedServer = server; }
	GameServer *GetSelectedServer() { return m_selectedServer; }

	bool GetPasswordFilter() const	{ return m_passwordFilter; }
	bool GetPunkbusterFilter() const{ return m_punkbusterFilter; }
	bool GetNoPunkbusterFilter() const { return m_NoPunkbusterFilter; }
	bool GetEmptyFilter() const		{ return m_emptyFilter; }
	bool GetFullFilter() const		{ return m_fullFilter; }
	bool GetTVFilter() const		{ return m_tvFilter; }
	const idStr GetProtocolFilter() const { return m_currentGame->GetProtocolFilter(); }
	const idStr GetFavouritesFilter() const { return m_currentGame->GetFavouritesFilter(); }

	bool GetUseICMPPing() const		{ return m_useICMPPing; }
	int GetQueryServerCount() const	{ return m_queryServerCount; }
	void SetUseICMPPing(bool newVal)	{ m_useICMPPing = newVal; }
	void SetQueryServerCount(int newVal) { m_queryServerCount = newVal; }

	void TogglePasswordFilter()		{ m_passwordFilter = !m_passwordFilter; }
	void TogglePunkbusterFilter()	{ m_punkbusterFilter = !m_punkbusterFilter; }
	void ToggleNoPunkbusterFilter()	{ m_NoPunkbusterFilter = !m_NoPunkbusterFilter; }
	void ToggleEmptyFilter()		{ m_emptyFilter = !m_emptyFilter; }
	void ToggleFullFilter()			{ m_fullFilter = !m_fullFilter; }
	void ToggleTVFilter()			{ m_tvFilter = !m_tvFilter; }

	void SetProtocolFilter(const char *val) { m_currentGame->SetProtocolFilter(val); }
	void SetFavouritesFilter(const char *val) { m_currentGame->SetFavouritesFilter(val); }

	void SetGame(const idStr &gameName);
	SupportedGame *GetGame(const idStr &gameName);
	SupportedGame *GetCurrentGame() const { return m_currentGame; }
	const idList<SupportedGame*> &GetSupportedGames() const { return m_supportedGames; }

	void AddServerToQuery(GameServer *thisServer);
	void QueryServers(CWnd *parentWnd = NULL);
	void CheckForNewServers(bool requeryExistingServers, CWnd *parentWnd = NULL);
	void ImportIPs(CString ipList);
	void DeleteOldServers();

	const IPNationalityMap &GetIPNationalityMap() { return m_nationalityMap; }
};


