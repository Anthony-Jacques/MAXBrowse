// ServerDetailsDlg.cpp : implementation file
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
#include "ServerDetailsDlg.h"
#include "ServerBrowserDoc.h"
#include "MainFrm.h"

#include "GameServer.h"
#include "PlayerDetails.h"
#include "SupportedGame.h"

// CServerDetailsDlg dialog

IMPLEMENT_DYNAMIC(CServerDetailsDlg, CDialog)
CServerDetailsDlg::CServerDetailsDlg(GameServer *theServer, CWnd* pParent /*=NULL*/)
	: CDialog(CServerDetailsDlg::IDD, pParent)
	, m_ip(_T(""))
	, m_ping(_T(""))
	, m_server(theServer)
	, m_name(_T(""))
	, m_playerCount(_T(""))
	, m_punkbuster(_T(""))
	, m_passworded(_T(""))
	, m_gametype(_T(""))
	, m_map(_T(""))
	, m_mod(_T(""))
{
	m_theGame = theApp.m_mainFrame->GetDocument()->GetCurrentGame();

	m_FirstTime = true;
}

CServerDetailsDlg::~CServerDetailsDlg()
{
}

void CServerDetailsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_IP, m_ip);
	DDX_Text(pDX, IDC_PING, m_ping);
	DDX_Control(pDX, IDC_SERVERINFO, m_serverInfo);
	DDX_Control(pDX, IDC_PLAYERS, m_players);
	DDX_Text(pDX, IDC_NAME, m_name);
	DDX_Text(pDX, IDC_PLAYER_COUNT, m_playerCount);
	DDX_Text(pDX, IDC_PUNKBUSTER, m_punkbuster);
	DDX_Text(pDX, IDC_PASSWORDED, m_passworded);
	DDX_Text(pDX, IDC_GAMETYPE, m_gametype);
	DDX_Text(pDX, IDC_MAP, m_map);
	DDX_Text(pDX, IDC_MOD, m_mod);
}


BEGIN_MESSAGE_MAP(CServerDetailsDlg, CDialog)
	ON_BN_CLICKED(IDC_REFRESH, OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_CONNECT, OnBnClickedConnect)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CServerDetailsDlg message handlers
BOOL CServerDetailsDlg::OnInitDialog()
{
	BOOL retVal = CDialog::OnInitDialog();

	CRect rect;
	m_serverInfo.GetClientRect(rect);

	int width = (rect.Width() - 18) / 2;
	m_serverInfo.InsertColumn(0, "Key", LVCFMT_LEFT, width);
	m_serverInfo.InsertColumn(1, "Value", LVCFMT_LEFT, width);
	m_serverInfo.SetExtendedStyle(LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT);

	idList<idStr> &playerColumns = m_theGame->GetPlayerDetails();
	int numPlayerColumns = playerColumns.Num();

	m_players.GetClientRect(rect);
	width = (rect.Width() - 18) / numPlayerColumns;
	for (int i=0; i<numPlayerColumns; i++)
        m_players.InsertColumn(i, playerColumns[i].c_str(), LVCFMT_LEFT, width);

	m_players.SetExtendedStyle(LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT);

	DisplayServer();

	return retVal;
}


void CServerDetailsDlg::OnBnClickedRefresh()
{
	if (m_server)
	{
		CServerBrowserDoc *theDoc = theApp.m_mainFrame->GetDocument();
		theDoc->AddServerToQuery(m_server);
		theDoc->QueryServers(this);
	}

	DisplayServer();
}

void CServerDetailsDlg::OnBnClickedConnect()
{
	CServerBrowserDoc *theDoc = theApp.m_mainFrame->GetDocument();
	theDoc->GetCurrentGame()->Connect(m_server);
}

void CServerDetailsDlg::DisplayServer()
{
	m_serverInfo.DeleteAllItems();
	m_players.DeleteAllItems();

	m_name = "";
	m_playerCount = "";
	m_punkbuster = "";
	m_passworded = "";
	m_gametype = "";
	m_map = "";

	if (!m_server)
		return;

	int numKeys = m_server->GetServerInfo().GetNumKeyVals();
	int item = 0;
	for (int j=0; j<numKeys; j++)
	{
		const idKeyValue *thisPair = m_server->GetServerInfo().GetKeyVal(j);
		
		item = m_serverInfo.InsertItem(item, thisPair->GetKey().c_str(), 0);
		m_serverInfo.SetItemText(item, 1, thisPair->GetValue().c_str());
		item++;
	}

	m_ping.Format("%d", m_server->GetPing());
	m_ip.Format("%s:%d", m_server->GetHost().c_str(), (int)m_server->GetPort());

	CString windowTitle = "Server Details: ";
	windowTitle += m_server->GetServerName();

	idList<idStr> &playerColumns = m_theGame->GetPlayerDetails();

	const idList<PlayerDetails*> &players = m_server->GetPlayers();
	int numPlayers = players.Num();
	item = 0;
	for (int j=0; j<numPlayers; j++)
	{
		const PlayerDetails *thisPlayer = players[j];

		item = m_players.InsertItem(item, thisPlayer->GetProperty(playerColumns[0].c_str()), 0);

		int numPlayerColumns = playerColumns.Num();
		for (int i=1; i<numPlayerColumns; i++)
			m_players.SetItemText(item, i, thisPlayer->GetProperty(playerColumns[i].c_str()));

		item++;
	}

	m_serverInfo.SortNow();
	m_players.SortNow();

	SupportedGame *currentGame = theApp.m_mainFrame->GetDocument()->GetCurrentGame();

	bool pb = m_server->GetServerInfo().GetBool(currentGame->GetPunkbusterCvar());
	bool pw = m_server->GetServerInfo().GetBool(currentGame->GetPasswordedCvar());

	m_punkbuster = pb ? "Yes" : "No";
	m_passworded = pw ? ((m_server->GetPassword().Length() > 0) ? "Yes (Stored)" : "Yes") : "No";
	m_gametype = m_server->GetGametype();
	m_name = m_server->GetServerName();
	m_map = m_server->GetMapName();
	m_playerCount = m_server->GetServerInfo().GetString("players");
	m_mod = m_server->GetServerInfo().GetString("gamename");

	SetWindowText(windowTitle);

	UpdateData(FALSE);
}
void CServerDetailsDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}


void CServerDetailsDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	// If it's been minimised, there is no point in moving things
	if(!IsIconic())
	{
		if (!m_FirstTime)
		{
			SetRedraw(FALSE);

			int deltaWidth = cx - m_LastCx;
			int deltaHeight = cy - m_LastCy;

			// adjust with width and height
			MoveObject(IDC_SERVERINFO, 0, 0, deltaWidth, deltaHeight);
			m_serverInfo.SetColumnWidth(1, m_serverInfo.GetColumnWidth(1) + deltaWidth);
			
			// stick to bottom right
			MoveObject(IDOK, deltaHeight, deltaWidth, 0, 0);

			// stick to bottom left
			MoveObject(IDC_REFRESH, deltaHeight, 0, 0, 0);
			MoveObject(IDC_CONNECT, deltaHeight, 0, 0, 0);

			// resize height but not width
			MoveObject(IDC_PLAYERS, 0, 0, 0, deltaHeight);

			SetRedraw(TRUE);

			Invalidate();
		}

		m_FirstTime = false;

		m_LastCx = cx;
		m_LastCy = cy;
	}
}

/// This moves a dialog control on the dialog by the given amount, taking into account the mess of screen and client
/// coordinates. In this case, the two coordinate systems are actually pretty close as we're a fullscreen dialog,
/// but its still worth being sure.
void CServerDetailsDlg::MoveObject(int aDlgId, int aDeltaTop, int aDeltaLeft, int aDeltaWidth, int aDeltaHeight)
{
	CWnd* lObject = GetDlgItem(aDlgId);

	CRect rect;
	lObject->GetWindowRect(&rect);

	rect.left += aDeltaLeft;
	rect.top += aDeltaTop;
	rect.right += aDeltaLeft + aDeltaWidth;
	rect.bottom += aDeltaTop + aDeltaHeight;

	ScreenToClient(rect);

	lObject->MoveWindow(rect);
}


void CServerDetailsDlg::OnGetMinMaxInfo(MINMAXINFO* aInfo)
{
	aInfo->ptMinTrackSize.x = 400;
	aInfo->ptMinTrackSize.y = 300;
}
