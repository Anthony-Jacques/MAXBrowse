// MainFrm.cpp : implementation of the CMainFrame class
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


#include "stdafx.h"
#include "ServerBrowser.h"
#include "ServerBrowserView.h"
#include "ServerBrowserDoc.h"
#include "ServerInfoView.h"
#include "FilterView.h"
#include "PlayerInfoView.h"

#include "MainFrm.h"

#include "SelectColumnsDlg.h"
#include "ServerDetailsDlg.h"
#include "ConfigureGamesDlg.h"
#include "CustomiseFilterDlg.h"
#include "PasswordDlg.h"
#include "FindServerDlg.h"
#include "AddServerDlg.h"
#include "SettingsDlg.h"

#include "GameServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_SYSTEMTRAY	(WM_APP + 1)


#define	COMBO_DROPHEIGHT		250
#define	PROTOCOL_COMBO_WIDTH	120
#define	FAVOURITES_COMBO_WIDTH	120

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_GAME_1, ID_GAME_100, OnGameSelected)
	ON_COMMAND(ID_GAME_SETTINGS, OnSettings)
	ON_COMMAND(ID_FILE_CONFIGUREGAMES, OnConfigureGames)
	ON_COMMAND(ID_GAME_FINDSERVER, OnFindServer)
	ON_COMMAND(ID_GAME_ADDSERVER, OnGameAddServer)
	ON_COMMAND(ID_SERVERLIST_IMPORTSERVERS, OnGameImportServers)
	ON_COMMAND(ID_GAME_CHECKFORNEWSERVERS, OnGameCheckfornewservers)
	ON_COMMAND(ID_GAME_REFRESHALLSERVERS, OnGameRefreshallservers)
	ON_COMMAND(ID_GAME_DELETEOLDSERVERS, OnGameDeleteOldServers)
	ON_COMMAND(ID_VIEW_EDITFILTERTREE, OnViewEditFilterTree)
	ON_COMMAND(ID_FILTER_SHOWPASSWORDED, OnShowPassworded)
	ON_COMMAND(ID_FILTER_SHOWPUNKBUSTER, OnShowPunkbuster)
	ON_COMMAND(ID_FILTER_SHOWWITHOUTPB, OnShowNoPunkbuster)
	ON_COMMAND(ID_FILTER_SHOWEMPTY, OnShowEmpty)
	ON_COMMAND(ID_FILTER_SHOWFULL, OnShowFull)
	ON_COMMAND(ID_FILTER_SHOWQ4TVSERVERS, OnShowTV)
	ON_COMMAND(ID_FILTER_SHOWPASSWORDED_MENU, OnShowPassworded)
	ON_COMMAND(ID_FILTER_SHOWPUNKBUSTER_MENU, OnShowPunkbuster)
	ON_COMMAND(ID_FILTER_SHOWWITHOUTPB_MENU, OnShowNoPunkbuster)
	ON_COMMAND(ID_FILTER_SHOWEMPTY_MENU, OnShowEmpty)
	ON_COMMAND(ID_FILTER_SHOWFULL_MENU, OnShowFull)
	ON_COMMAND(ID_FILTER_SHOWQ4TVSERVERS_MENU, OnShowTV)
	ON_COMMAND(ID_VIEW_SELECTCOLUMNS, OnSelectColumns)
	ON_COMMAND(ID_SERVER_VIEWDETAILS, OnServerViewDetails)
	ON_COMMAND(ID_SERVER_CONNECT, OnServerConnect)
	ON_COMMAND(ID_SERVER_REFRESH, OnServerRefresh)
	ON_COMMAND(ID_SERVER_REFRESHVISIBLE, OnServerRefreshVisible)
	ON_COMMAND(ID_SERVER_SETPASSWORD, OnServerSetPassword)
	ON_COMMAND(ID_SERVER_ADDTOFAVOURITES, OnServerAddToFavourites)
	ON_COMMAND(ID_SERVER_REMOVEFROMFAVOURITES, OnServerRemoveFromFavourites)
	ON_COMMAND(ID_EDIT_COPY_IP, OnEditCopyIP)
	ON_COMMAND(ID_EDIT_COPYDETAILS, OnEditCopyDetails)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnSelectAll)
	ON_UPDATE_COMMAND_UI(ID_GAME_ADDSERVER, OnUpdateGameSelected)
	ON_UPDATE_COMMAND_UI(ID_SERVERLIST_IMPORTSERVERS, OnUpdateGameSelected)
	ON_UPDATE_COMMAND_UI(ID_GAME_CHECKFORNEWSERVERS, OnUpdateGameSelected)
	ON_UPDATE_COMMAND_UI(ID_GAME_REFRESHALLSERVERS, OnUpdateGameSelected)
	ON_UPDATE_COMMAND_UI(ID_SERVER_REMOVEFROMFAVOURITES, OnUpdateRemoveFavourites)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPYDETAILS, ServerUniquelySelected)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY_IP, ServerUniquelySelected)
	ON_UPDATE_COMMAND_UI(ID_FILTER_SHOWPASSWORDED, OnUpdateShowPassworded)
	ON_UPDATE_COMMAND_UI(ID_FILTER_SHOWPUNKBUSTER, OnUpdateShowPunkbuster)
	ON_UPDATE_COMMAND_UI(ID_FILTER_SHOWWITHOUTPB, OnUpdateShowNoPunkbuster)
	ON_UPDATE_COMMAND_UI(ID_FILTER_SHOWEMPTY, OnUpdateShowEmpty)
	ON_UPDATE_COMMAND_UI(ID_FILTER_SHOWFULL, OnUpdateShowFull)
	ON_UPDATE_COMMAND_UI(ID_FILTER_SHOWQ4TVSERVERS, OnUpdateShowTV)
	ON_UPDATE_COMMAND_UI(ID_FILTER_SHOWPASSWORDED_MENU, OnUpdateShowPassworded)
	ON_UPDATE_COMMAND_UI(ID_FILTER_SHOWPUNKBUSTER_MENU, OnUpdateShowPunkbuster)
	ON_UPDATE_COMMAND_UI(ID_FILTER_SHOWWITHOUTPB_MENU, OnUpdateShowNoPunkbuster)
	ON_UPDATE_COMMAND_UI(ID_FILTER_SHOWEMPTY_MENU, OnUpdateShowEmpty)
	ON_UPDATE_COMMAND_UI(ID_FILTER_SHOWFULL_MENU, OnUpdateShowFull)
	ON_UPDATE_COMMAND_UI(ID_FILTER_SHOWQ4TVSERVERS_MENU, OnUpdateShowTV)
	ON_UPDATE_COMMAND_UI(ID_SERVER_REFRESH, ServerSelected)
	ON_UPDATE_COMMAND_UI(ID_SERVER_REFRESHVISIBLE, ServersListed)
	ON_UPDATE_COMMAND_UI(ID_SERVER_ADDTOFAVOURITES, ServerSelected)
	ON_UPDATE_COMMAND_UI(ID_SERVER_VIEWDETAILS, ServerUniquelySelected)
	ON_UPDATE_COMMAND_UI(ID_SERVER_CONNECT, ServerUniquelySelected)
	ON_UPDATE_COMMAND_UI(ID_SERVER_SETPASSWORD, ServerUniquelySelected)
	ON_UPDATE_COMMAND_UI_RANGE(ID_GAME_1, ID_GAME_100, OnUpdateGame)
	ON_CBN_SELCHANGE(ID_PROTOCOL_COMBO, OnSelChangeProtocolFilterCombo)
	ON_CBN_SELCHANGE(ID_FAVOURITES_COMBO, OnSelChangeFavouritesFilterCombo)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_SYSTEMTRAY, OnSystemTray)
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_SERVERS,
	ID_INDICATOR_PLAYERS
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	theApp.m_mainFrame = this;
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

//	SetMenu(NULL);
	//m_wndMenu.LoadMenu(IDR_MAINFRAME);

//	if (!m_wndMenuBar.CreateEx(this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_RAISEDBORDER) ||
//		!m_wndMenuBar.LoadMenuBar(IDR_MAINFRAME))
// 

	if (0)
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}	

	if (!m_wndToolBar.CreateEx(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndToolBar))
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}

	CRect comboRect;
	m_wndToolBar.GetToolBarCtrl().GetItemRect(m_wndToolBar.GetToolBarCtrl().CommandToIndex(ID_PROTOCOL_COMBO), &comboRect);
	comboRect.top = 1;
	comboRect.bottom = comboRect.top + COMBO_DROPHEIGHT;
	comboRect.right = comboRect.left + PROTOCOL_COMBO_WIDTH;
	if(!m_protocolCombo.Create(CBS_DROPDOWNLIST | CBS_SORT | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL, comboRect, &m_wndToolBar, ID_PROTOCOL_COMBO))
	{
		TRACE(_T("Failed to create combo-box\n"));
		return FALSE;
	}

	m_wndToolBar.GetToolBarCtrl().GetItemRect(m_wndToolBar.GetToolBarCtrl().CommandToIndex(ID_FAVOURITES_COMBO), &comboRect);
	comboRect.top = 1;
	comboRect.bottom = comboRect.top + COMBO_DROPHEIGHT;
	comboRect.right = comboRect.left + FAVOURITES_COMBO_WIDTH;

	if(!m_favouritesCombo.Create(CBS_DROPDOWNLIST | CBS_SORT | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL, comboRect, &m_wndToolBar, ID_FAVOURITES_COMBO))
	{
		TRACE(_T("Failed to create combo-box\n"));
		return FALSE;
	}

	CFont font;
	font.CreateStockObject(DEFAULT_GUI_FONT);
	m_protocolCombo.SetFont(&font);
	m_favouritesCombo.SetFont(&font);
	font.DeleteObject();

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

    TEXTMETRIC tm;
    CClientDC dc(this);
    CFont* pFont = GetFont();
    CFont* pOldFont = dc.SelectObject(pFont);
    dc.GetTextMetrics(&tm);
    dc.SelectObject(pOldFont);

    int cxWidth;
    UINT nID, nStyle;
    m_wndStatusBar.GetPaneInfo (1, nID, nStyle, cxWidth);
	m_wndStatusBar.SetPaneInfo (1, nID, nStyle, tm.tmAveCharWidth * (int)strlen("Servers: 1000"));

    m_wndStatusBar.GetPaneInfo (2, nID, nStyle, cxWidth);
	m_wndStatusBar.SetPaneInfo (2, nID, nStyle, tm.tmAveCharWidth * (int)strlen("Players: 1000+100"));

	if (m_minimiseOnStartup)
	{
		if (m_minimiseToSysTray)
			MinimiseToTray();
		else
			ShowWindow(SW_MINIMIZE);
	}

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	CRect windowSize;
	GetClientRect(windowSize);
	int width = windowSize.Width();
	int height = windowSize.Height();

	if (!m_parentSplitter.CreateStatic(this, 1, 2))
	{
		MessageBox("Could not make first split view");
		return FALSE;
	}

	if (!m_childSplitter.CreateStatic( &m_parentSplitter, 3, 1,
									WS_CHILD | WS_VISIBLE | WS_BORDER, m_parentSplitter.IdFromRowCol(0, 0)))
	{
		AfxMessageBox("Could not make second split view");
		return FALSE;
	}

	m_minimiseToSysTray = theApp.GetProfileInt("Settings", "MinimiseToSysTray", false);
	m_minimiseOnConnect = theApp.GetProfileInt("Settings", "MinimiseOnConnect", false);
	m_minimiseOnStartup = theApp.GetProfileInt("Settings", "MinimiseOnStartup", false);
	m_closeOnConnect = theApp.GetProfileInt("Settings", "CloseOnConnect", false);

	int filterTreeHeight = theApp.GetProfileInt("Positions", "FilterTreeHeight", height / 3);
	int filterTreeWidth = theApp.GetProfileInt("Positions", "FilterTreeWidth", 200);
	int serverInfoHeight = theApp.GetProfileInt("Positions", "ServerInfoHeight", 4 * height / 6);

	m_parentSplitter.SetColumnInfo(0, filterTreeWidth, 0);
	m_childSplitter.SetRowInfo(0, filterTreeHeight, 0);
	m_childSplitter.SetRowInfo(1, serverInfoHeight, 0);

	if (!m_parentSplitter.CreateView(0, 1, RUNTIME_CLASS(CServerBrowserView), CSize(width - filterTreeWidth, height), pContext))
	{
		AfxMessageBox("Could not make Layout View");
		return FALSE;
	}

	if (!m_childSplitter.CreateView(0, 0, RUNTIME_CLASS(CFilterView), CSize(filterTreeWidth, filterTreeHeight), pContext))
	{
		AfxMessageBox("Could not make filter view");
		return FALSE;
	}

	if (!m_childSplitter.CreateView(1, 0, RUNTIME_CLASS(CServerInfoView), CSize(filterTreeWidth, serverInfoHeight), pContext))
	{
		AfxMessageBox("Could not make ServerInfo view");
		return FALSE;
	}

	if (!m_childSplitter.CreateView(2, 0, RUNTIME_CLASS(CPlayerInfoView), CSize(filterTreeWidth, height - filterTreeHeight - serverInfoHeight), pContext))
	{
		AfxMessageBox("Could not make PlayerInfo view");
		return FALSE;
	}

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.x = theApp.GetProfileInt("Positions", "left", 30);
	cs.cx = theApp.GetProfileInt("Positions","width", 600);
	cs.y = theApp.GetProfileInt("Positions", "top", 30);
	cs.cy = theApp.GetProfileInt("Positions", "height", 400);

	return TRUE;
}

void CMainFrame::OnDestroy() 
{
	CFrameWnd::OnDestroy();
	
	WINDOWPLACEMENT placement;
	GetWindowPlacement(&placement);
	
	theApp.WriteProfileInt("Positions", "left", placement.rcNormalPosition.left);
	theApp.WriteProfileInt("Positions", "width", placement.rcNormalPosition.right - placement.rcNormalPosition.left);
	theApp.WriteProfileInt("Positions", "top", placement.rcNormalPosition.top);
	theApp.WriteProfileInt("Positions", "height", placement.rcNormalPosition.bottom - placement.rcNormalPosition.top);

	theApp.WriteProfileInt("Positions", "IsMaximised", placement.showCmd == SW_MAXIMIZE);

	int min, value;
	m_parentSplitter.GetColumnInfo(0, value, min);
	theApp.WriteProfileInt("Positions", "FilterTreeWidth", value);

	m_childSplitter.GetRowInfo(0, value, min);
	theApp.WriteProfileInt("Positions", "FilterTreeHeight", value);

	m_childSplitter.GetRowInfo(1, value, min);
	theApp.WriteProfileInt("Positions", "ServerInfoHeight", value);

	theApp.WriteProfileInt("Settings", "MinimiseToSysTray", m_minimiseToSysTray);
	theApp.WriteProfileInt("Settings", "MinimiseOnConnect", m_minimiseOnConnect);
	theApp.WriteProfileInt("Settings", "MinimiseOnStartup", m_minimiseOnStartup);
	theApp.WriteProfileInt("Settings", "CloseOnConnect", m_closeOnConnect);
}

CFilterView* CMainFrame::GetFilterView()
{
	CWnd* pWnd = m_childSplitter.GetPane(0, 0);
	CFilterView* pView = DYNAMIC_DOWNCAST(CFilterView, pWnd);
	return pView;
}

CServerBrowserView* CMainFrame::GetServerBrowserView()
{
	CWnd* pWnd = m_parentSplitter.GetPane(0, 1);
	CServerBrowserView* pView = DYNAMIC_DOWNCAST(CServerBrowserView, pWnd);
	return pView;
}

CServerInfoView* CMainFrame::GetServerInfoView()
{
	CWnd* pWnd = m_childSplitter.GetPane(1, 0);
	CServerInfoView* pView = DYNAMIC_DOWNCAST(CServerInfoView, pWnd);
	return pView;
}

CPlayerInfoView* CMainFrame::GetPlayerInfoView()
{
	CWnd* pWnd = m_childSplitter.GetPane(2, 0);
	CPlayerInfoView* pView = DYNAMIC_DOWNCAST(CPlayerInfoView, pWnd);
	return pView;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

void CMainFrame::PopulateGameMenu()
{
	CMenu* mainMenu = GetMenu();
	if (mainMenu)
	{
		CMenu* gameMenu = mainMenu->GetSubMenu(0);
		if (gameMenu)
		{
			for (int i = 0; i < 100; i++)
				gameMenu->RemoveMenu(ID_GAME_1 + i, MF_BYCOMMAND);

			for (int i = 0; i < GetDocument()->GetSupportedGames().Num(); i++)
			{
				SupportedGame* theGame = GetDocument()->GetSupportedGames()[i];
				if (!theGame->IsInstalled())
					continue;

				MENUITEMINFO menuItemInfo;
				menuItemInfo.fMask = /*MIIM_STRING | */ MIIM_ID | MIIM_DATA | MIIM_TYPE;
				menuItemInfo.fType = MFT_STRING;
				menuItemInfo.dwTypeData = (char*)theGame->GetName().c_str();
				menuItemInfo.cch = strlen(menuItemInfo.dwTypeData);

				menuItemInfo.wID = ID_GAME_1 + i;
				menuItemInfo.cbSize = sizeof(MENUITEMINFO);

				gameMenu->InsertMenuItem(0, &menuItemInfo, MF_BYPOSITION);
			}
		}
	}
}

// CMainFrame message handlers
void CMainFrame::OnUpdateShowEmpty(CCmdUI* pCmdUI)		{ pCmdUI->SetCheck(GetDocument()->GetEmptyFilter()); }
void CMainFrame::OnUpdateShowFull(CCmdUI* pCmdUI)		{ pCmdUI->SetCheck(GetDocument()->GetFullFilter()); }
void CMainFrame::OnUpdateShowPunkbuster(CCmdUI* pCmdUI) { pCmdUI->SetCheck(GetDocument()->GetPunkbusterFilter()); }
void CMainFrame::OnUpdateShowNoPunkbuster(CCmdUI* pCmdUI) { pCmdUI->SetCheck(GetDocument()->GetNoPunkbusterFilter()); }
void CMainFrame::OnUpdateShowPassworded(CCmdUI* pCmdUI) { pCmdUI->SetCheck(GetDocument()->GetPasswordFilter()); }
void CMainFrame::OnUpdateShowTV(CCmdUI* pCmdUI) { pCmdUI->SetCheck(GetDocument()->GetTVFilter()); }

void CMainFrame::ServerSelected(CCmdUI* pCmdUI) { pCmdUI->Enable(GetServerBrowserView()->GetListCtrl().GetSelectedCount() >= 1); }
void CMainFrame::ServerUniquelySelected(CCmdUI* pCmdUI) { pCmdUI->Enable(GetServerBrowserView()->GetListCtrl().GetSelectedCount() == 1); }
void CMainFrame::ServersListed(CCmdUI* pCmdUI) { pCmdUI->Enable(GetServerBrowserView()->GetListCtrl().GetItemCount() > 0); }


void CMainFrame::OnUpdateGameSelected(CCmdUI *pCmdUI)	{ pCmdUI->Enable(GetDocument()->GetCurrentGame() != NULL); }



void CMainFrame::OnUpdateRemoveFavourites(CCmdUI* pCmdUI) 
{ 
	CString favsVal;
	int curSel = m_favouritesCombo.GetCurSel();
	if (curSel > -1)
	{
		m_favouritesCombo.GetLBText(curSel, favsVal);
		if (favsVal == FAVOURITES_NO_FILTER)
			favsVal = "";
	}

	pCmdUI->Enable(favsVal.GetLength() > 0 && GetServerBrowserView()->GetListCtrl().GetSelectedCount() >= 1);
}

void CMainFrame::OnUpdateFrameMenu(HMENU hMenuAlt)
{
    CFrameWnd::OnUpdateFrameMenu(hMenuAlt); 
}


void CMainFrame::OnShowPassworded()
{
	GetDocument()->TogglePasswordFilter();
	GetDocument()->FilterServerList();

	GetDocument()->UpdateAllViews(NULL, VIEW_FILTER_CHANGED);
}

void CMainFrame::OnShowPunkbuster()
{
	GetDocument()->TogglePunkbusterFilter();
	if (!GetDocument()->GetNoPunkbusterFilter())
		GetDocument()->ToggleNoPunkbusterFilter();

	GetDocument()->FilterServerList();

	GetDocument()->UpdateAllViews(NULL, VIEW_FILTER_CHANGED);
}

void CMainFrame::OnShowNoPunkbuster()
{
	GetDocument()->ToggleNoPunkbusterFilter();
	if (!GetDocument()->GetPunkbusterFilter())
		GetDocument()->TogglePunkbusterFilter();

	GetDocument()->FilterServerList();

	GetDocument()->UpdateAllViews(NULL, VIEW_FILTER_CHANGED);
}

void CMainFrame::OnShowEmpty()
{
	GetDocument()->ToggleEmptyFilter();
	GetDocument()->FilterServerList();

	GetDocument()->UpdateAllViews(NULL, VIEW_FILTER_CHANGED);
}

void CMainFrame::OnShowFull()
{
	GetDocument()->ToggleFullFilter();
	GetDocument()->FilterServerList();

	GetDocument()->UpdateAllViews(NULL, VIEW_FILTER_CHANGED);
}


void CMainFrame::OnShowTV()
{
	GetDocument()->ToggleTVFilter();
	GetDocument()->FilterServerList();

	GetDocument()->UpdateAllViews(NULL, VIEW_FILTER_CHANGED);
}


void CMainFrame::OnSelectColumns()
{
	CSelectColumnsDlg dlg(this);
	if (dlg.DoModal() == IDOK)
	{
		GetDocument()->UpdateAllViews(NULL, VIEW_COLUMNS_CHANGED);
	}
}

void CMainFrame::OnViewEditFilterTree()
{
	CCustomiseFilterDlg dlg(this);
	if (dlg.DoModal() == IDOK)
	{
		GetDocument()->FilterServerList();
		GetDocument()->UpdateAllViews(NULL, VIEW_FILTER_TREE_CHANGED);
	}
}

void CMainFrame::OnServerViewDetails()
{
	CServerDetailsDlg dlg(GetDocument()->GetSelectedServer(), this);
	dlg.DoModal();
}

void CMainFrame::OnServerConnect()
{
	CServerBrowserDoc *theDoc = GetDocument();
	GameServer *gameServer = theDoc->GetSelectedServer();

	const SupportedGame *theGame = theDoc->GetCurrentGame();

	if (theGame->Connect(gameServer))
	{
		if (m_closeOnConnect)
		{
			PostMessage(WM_CLOSE);
		}
		else if (m_minimiseOnConnect)
		{
			if (m_minimiseToSysTray)
				MinimiseToTray();
			else
				ShowWindow(SW_MINIMIZE);
		}
	}
}

void CMainFrame::OnServerRefreshVisible()
{
	GetServerBrowserView()->RefreshVisible();
}

void CMainFrame::OnServerRefresh()
{
	GetServerBrowserView()->RefreshSelected();
}

void CMainFrame::OnServerSetPassword()
{
	GameServer *thisServer = GetDocument()->GetSelectedServer();

	CPasswordDlg dlg(this);
	dlg.m_server = va("%s:%d", thisServer->GetHost().c_str(), (int)thisServer->GetPort());
	dlg.m_name = thisServer->GetServerName();
	dlg.m_password = thisServer->GetPassword().c_str();

	if (dlg.DoModal() == IDOK)
		thisServer->SetPassword(dlg.m_password);
}

void CMainFrame::OnServerAddToFavourites()
{
	GetServerBrowserView()->AddToFavourites();
}

void CMainFrame::OnServerRemoveFromFavourites()
{
	GetServerBrowserView()->RemoveFromFavourites();
}

void CMainFrame::CopyText(const CString &source)
{
	if(OpenClipboard())
	{
		HGLOBAL clipbuffer;
		char * buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, source.GetLength()+1);
		buffer = (char*)GlobalLock(clipbuffer);
		strcpy(buffer, LPCSTR(source));
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);
		CloseClipboard();
	}
}

void CMainFrame::OnEditCopyIP()
{
	GameServer *server = GetDocument()->GetSelectedServer();
	CString serverIP;
	serverIP.Format("%s:%d", server->GetHost().c_str(), (int)server->GetPort());

	CopyText(serverIP);
}

void CMainFrame::OnEditCopyDetails()
{
	
	GameServer *server = GetDocument()->GetSelectedServer();
	CString serverDetails;
	serverDetails.Format("IP: %s:%d | Ping: %d | Name: %s | Mod: %s | GameType: %s | Map: %s | %s: %s", 
					server->GetHost().c_str(), (int)server->GetPort(), 
//					server->GetPassword().Length() > 0 ? va(" Pass: %s |", server->GetPassword().c_str()) : "",
					server->GetPing(),
					server->GetServerName(), 
					server->GetMod(),
					server->GetGametype(),
					server->GetMapName(),
					server->GetServerInfo().GetBool("si_tv") ? "Viewers" : "Players",
					server->GetServerInfo().GetString("players"));

	CopyText(serverDetails);
}

void CMainFrame::UpdateStatusBar(int servers, int players, int bots)
{
	CString serversStr, playersStr;
	serversStr.Format("Servers: %d", servers);
	if (bots > 0)
		playersStr.Format("Players: %d+%d", players, bots);
	else
		playersStr.Format("Players: %d", players);

	m_wndStatusBar.SetPaneText(1, serversStr, TRUE);
	m_wndStatusBar.SetPaneText(2, playersStr, TRUE);
}

void CMainFrame::OnGameSelected(UINT nID)
{
	SupportedGame *thisGame = GetDocument()->GetSupportedGames()[nID - ID_GAME_1];
	GetDocument()->SetGame(thisGame->GetName());
}

void CMainFrame::OnUpdateGame(CCmdUI* pCmdUI)
{
	SupportedGame *thisGame = GetDocument()->GetSupportedGames()[pCmdUI->m_nID - ID_GAME_1];
	pCmdUI->SetCheck(GetDocument()->GetCurrentGame() == thisGame);
}

void CMainFrame::OnSettings()
{
	CSettingsDlg dlg(this);
	dlg.m_minimiseToSystray = m_minimiseToSysTray;
	dlg.m_minimiseOnConnect = m_minimiseOnConnect;
	dlg.m_minimiseOnStartup = m_minimiseOnStartup;
	dlg.m_closeOnConnect = m_closeOnConnect;
	dlg.m_useICMP = GetDocument()->GetUseICMPPing();
	dlg.m_queryCount = GetDocument()->GetQueryServerCount();
	dlg.m_shadeAlternateRows = GetServerBrowserView()->ShadeAlternateRows();
	dlg.m_favouritesColour.SetColour(GetServerBrowserView()->GetFavColour());
	dlg.m_fullColour.SetColour(GetServerBrowserView()->GetFullColour());

	if (dlg.DoModal() == IDOK)
	{
		m_minimiseToSysTray = dlg.m_minimiseToSystray;
		m_minimiseOnConnect = dlg.m_minimiseOnConnect;
		m_minimiseOnStartup = dlg.m_minimiseOnStartup;
		m_closeOnConnect = dlg.m_closeOnConnect;
		GetDocument()->SetUseICMPPing(dlg.m_useICMP);
		GetDocument()->SetQueryServerCount(dlg.m_queryCount);
		GetServerBrowserView()->ShadeAlternateRows(dlg.m_shadeAlternateRows);
		GetServerInfoView()->ShadeAlternateRows(dlg.m_shadeAlternateRows);
		GetPlayerInfoView()->ShadeAlternateRows(dlg.m_shadeAlternateRows);
		GetServerBrowserView()->SetFavColour(dlg.m_favouritesColour.GetColour());
		GetServerBrowserView()->SetFullColour(dlg.m_fullColour.GetColour());
	}
}

void CMainFrame::OnConfigureGames()
{
	CConfigureGamesDlg dlg(GetDocument()->GetSupportedGames(), "Configure Games", this);
	dlg.DoModal();

	if (!GetDocument()->GetCurrentGame() || !GetDocument()->GetCurrentGame()->IsInstalled())
	{
		for (int i=0; i<GetDocument()->GetSupportedGames().Num(); i++)
		{
			GetDocument()->SetGame(GetDocument()->GetSupportedGames()[i]->GetName());
			if (GetDocument()->GetCurrentGame())
				break;
		}
	}

	PopulateGameMenu();
}

void CMainFrame::OnGameAddServer()
{
	CAddServerDlg addServerDlg(this);
	if (addServerDlg.DoModal() == IDOK)
	{
		CServerDetailsDlg dlg(addServerDlg.m_theServer, this);
		dlg.DoModal();

		GetDocument()->UpdateAllViews(NULL, SERVERS_REFRESHED);
	}
}


void CMainFrame::OnGameImportServers()
{
	static char szFilter[] = "Text files (*.txt)|*.txt|All files|*.*||";

	CFileDialog fileDlg(TRUE, ".txt", "servers.txt", OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, szFilter, this);
	if (fileDlg.DoModal() == IDOK)
	{
		CStdioFile textFile;
		if (textFile.Open(fileDlg.GetPathName(), CFile::OpenFlags::modeRead | CFile::OpenFlags::shareDenyWrite))
		{
			CString line, ipList;
			while (textFile.ReadString(line))
			{
				ipList += line;
				ipList += "\n";
			}
			textFile.Close();

			this->GetDocument()->ImportIPs(ipList);
		}
		else
		{
			CString message;
			message.Format("Unable to open %s to import servers", fileDlg.GetFileName());
			AfxMessageBox(message);
		}
	}
}


void CMainFrame::OnGameCheckfornewservers()
{
	GetDocument()->CheckForNewServers(false, this);
}



void CMainFrame::OnGameRefreshallservers()
{
	GetDocument()->CheckForNewServers(true, this);
}

void CMainFrame::OnGameDeleteOldServers()
{
	GetDocument()->DeleteOldServers();
}

void CMainFrame::OnFindServer()
{
	CFindServerDlg dlg(this);
	if (dlg.DoModal() == IDOK)
		GetServerBrowserView()->SelectServers((const char*)dlg.m_searchText, dlg.m_nameOnly, dlg.m_caseSensitive);
}

void CMainFrame::OnSelectAll()
{
	GetServerBrowserView()->SelectAllServers();
}

void CMainFrame::OnSelChangeProtocolFilterCombo()
{
	CString protocolFilter;
	m_protocolCombo.GetLBText(m_protocolCombo.GetCurSel(), protocolFilter);
	if (protocolFilter == "<All versions>")
		protocolFilter = "";

	GetDocument()->SetProtocolFilter(protocolFilter);
	GetDocument()->FilterServerList();
	GetDocument()->UpdateAllViews(NULL, VIEW_FILTER_CHANGED);
}

void CMainFrame::OnSelChangeFavouritesFilterCombo()
{
	CString favouritesFilter;
	m_favouritesCombo.GetLBText(m_favouritesCombo.GetCurSel(), favouritesFilter);
	if (favouritesFilter == FAVOURITES_NO_FILTER)
		favouritesFilter = "";

	GetDocument()->SetFavouritesFilter(favouritesFilter);
	GetDocument()->FilterServerList();
	GetDocument()->UpdateAllViews(NULL, VIEW_FILTER_CHANGED);
}

void CMainFrame::MinimiseToTray()
{
	ShowWindow(SW_MINIMIZE);

	NOTIFYICONDATA tnd;
	tnd.cbSize = sizeof(NOTIFYICONDATA);
	tnd.hWnd = GetSafeHwnd();
	tnd.uID = 0;
	tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	tnd.uCallbackMessage = WM_SYSTEMTRAY;
	strcpy (tnd.szTip, "MAXBrowse");
	tnd.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	Shell_NotifyIcon(NIM_ADD, &tnd);

	ShowWindow(SW_HIDE);
}

void CMainFrame::RestoreFromTray()
{
	ShowWindow(SW_SHOW);

	NOTIFYICONDATA tnd;    // Tray notification data.
	tnd.cbSize = sizeof(NOTIFYICONDATA);
	tnd.hWnd = GetSafeHwnd();
	tnd.uID = 0;
	tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	tnd.uCallbackMessage = WM_SYSTEMTRAY;
	Shell_NotifyIcon(NIM_DELETE, &tnd);

	ShowWindow(SW_RESTORE);
}

LRESULT CMainFrame::OnSystemTray( WPARAM wParam, LPARAM lParam )
{
	if (lParam == WM_LBUTTONDOWN)
		RestoreFromTray();

	return TRUE;
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_MINIMIZE)
	{
		if (m_minimiseToSysTray)
			MinimiseToTray();
		else
			CFrameWnd::OnSysCommand(nID, lParam);
	}
	else
	{
		CFrameWnd::OnSysCommand(nID, lParam);
	}
	
}
