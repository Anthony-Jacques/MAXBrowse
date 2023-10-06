// MainFrm.h : interface of the CMainFrame class
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


class CServerBrowserDoc;

class CServerInfoView;
class CServerBrowserView;
class CFilterView;
class CPlayerInfoView;


#define	FAVOURITES_NO_FILTER	"<Favourite groups>"

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
protected:
	CSplitterWnd m_parentSplitter;
	CSplitterWnd m_childSplitter;
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdateFrameMenu(HMENU hMenuAlt);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CMenu	m_wndMenu;
	CToolBar	m_wndToolBar;

	CComboBox	m_protocolCombo;
	CComboBox	m_favouritesCombo;

	CReBar      m_wndReBar;
	bool		m_minimiseToSysTray;
	bool		m_minimiseOnConnect;
	bool		m_minimiseOnStartup;
	bool		m_closeOnConnect;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

public:
	void PopulateGameMenu();

	void OnSelectColumns();
	void OnViewEditFilterTree();

	void OnShowPassworded();
	void OnShowPunkbuster();
	void OnShowNoPunkbuster();
	void OnShowEmpty();
	void OnShowFull();
	void OnShowTV();
	void OnShowOnlyFavourites();

	void OnServerViewDetails();
	void OnServerConnect();
	void OnServerRefresh();
	void OnServerRefreshVisible();
	void OnServerSetPassword();
	void OnServerAddToFavourites();
	void OnServerRemoveFromFavourites();

	void OnEditCopyIP();
	void OnEditCopyDetails();
	void OnSelectAll();

	void OnGameSelected(UINT nID);
	void OnGameAddServer();
	void OnGameImportServers();
	void OnGameCheckfornewservers();
	void OnUpdateGameSelected(CCmdUI *pCmdUI);
	void OnGameRefreshallservers();
	void OnGameDeleteOldServers();
	void OnFindServer();

	void OnSettings();
	void OnConfigureGames();

	void OnUpdateGame(CCmdUI* pCmdUI);
	void OnUpdateShowPassworded(CCmdUI* pCmdUI);
	void OnUpdateShowPunkbuster(CCmdUI* pCmdUI);
	void OnUpdateShowNoPunkbuster(CCmdUI* pCmdUI);
	void OnUpdateShowEmpty(CCmdUI* pCmdUI);
	void OnUpdateShowFull(CCmdUI* pCmdUI);
	void OnUpdateShowTV(CCmdUI* pCmdUI);
	void OnUpdateShowOnlyFavourites(CCmdUI* pCmdUI);
	void ServerUniquelySelected(CCmdUI* pCmdUI);
	void ServersListed(CCmdUI* pCmdUI);
	void ServerSelected(CCmdUI* pCmdUI);

	void OnSelChangeProtocolFilterCombo();
	void OnSelChangeFavouritesFilterCombo();
	void OnUpdateRemoveFavourites(CCmdUI* pCmdUI);

	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID,LPARAM lParam);

	LRESULT OnSystemTray(WPARAM wParam, LPARAM lParam);
	void RestoreFromTray();
	void MinimiseToTray();

	CServerBrowserDoc *GetDocument() { return (CServerBrowserDoc*)GetActiveDocument(); }
	CServerInfoView* GetServerInfoView();
	CPlayerInfoView* GetPlayerInfoView();
	CServerBrowserView* GetServerBrowserView();
	CFilterView* GetFilterView();

	CComboBox *GetProtocolCombo() { return &m_protocolCombo; }
	CComboBox *GetFavouritesCombo() { return &m_favouritesCombo; }

	void UpdateStatusBar(int servers, int players, int bots);

	void CopyText(const CString &source);
};