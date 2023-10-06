// AddServerDlg.cpp : implementation file
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
#include "AddServerDlg.h"
#include ".\addserverdlg.h"

#include "MainFrm.h"
#include "ServerBrowserDoc.h"
#include "MasterServer.h"
#include "GameServer.h"


#define	NOT_A_FAVOURITE	"<Not a favourite>"

// CAddServerDlg dialog

IMPLEMENT_DYNAMIC(CAddServerDlg, CDialog)
CAddServerDlg::CAddServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddServerDlg::IDD, pParent)
	, m_hostname(_T(""))
	, m_favouritesStr("")
{
	m_theServer = NULL;
	m_dottedQuad[0] = 0;
	m_port = theApp.m_mainFrame->GetDocument()->GetCurrentGame()->GetDefaultGamePort();
}

CAddServerDlg::~CAddServerDlg()
{
}

void CAddServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_HOSTNAME, m_hostname);
	DDX_Control(pDX, IDC_FAVOURITES_COMBO, m_favourites);
	DDX_Text(pDX, IDC_FAVOURITES_COMBO, m_favouritesStr);
}


BEGIN_MESSAGE_MAP(CAddServerDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CAddServerDlg message handlers

BOOL CAddServerDlg::OnInitDialog()
{
	BOOL retVal = CDialog::OnInitDialog();

	CMainFrame *mainframe = (CMainFrame *)AfxGetMainWnd();
	CComboBox *mainFavsCombo = mainframe->GetFavouritesCombo();

	m_favourites.AddString(NOT_A_FAVOURITE);
	int countFavs = mainFavsCombo->GetCount();
	for (int i=0; i<countFavs; i++)
	{
		CString name;
		mainFavsCombo->GetLBText(i, name);

		if (name != FAVOURITES_NO_FILTER)
			m_favourites.AddString(name);
	}

	m_favourites.SelectString(-1, NOT_A_FAVOURITE);

	return retVal;
}


void CAddServerDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	int colon = m_hostname.Find(":");
	if (colon > -1)
	{
		m_port = atoi(m_hostname.Right(m_hostname.GetLength() - colon - 1));
		m_hostname = m_hostname.Left(colon);
	}
	else
	{
		m_port = theApp.m_mainFrame->GetDocument()->GetCurrentGame()->GetDefaultGamePort();
	} 

	hostent *host = gethostbyname(m_hostname);
	if (!host)
	{
		AfxMessageBox("Invalid hostname - please check for typos");
		return;
	}

	unsigned char *ipAddress = (unsigned char *)host->h_addr_list[0];
	
	sprintf(m_dottedQuad, "%d.%d.%d.%d", (int)ipAddress[0], (int)ipAddress[1], (int)ipAddress[2], (int)ipAddress[3]);

	CServerBrowserDoc *theDoc = theApp.m_mainFrame->GetDocument();
	MasterServer *currentMaster = theDoc->GetCurrentGame()->GetMasterServer();
	GameServer *theServer = currentMaster->GetServer(m_dottedQuad, m_port);
	if (!theServer)
	{
		theServer = currentMaster->AddServer(m_dottedQuad, m_port);

		theDoc->AddServerToQuery(theServer);
		theDoc->QueryServers(this);

		if (theServer->GetPing() == PING_TIMEOUT)
		{
			if (AfxMessageBox("No response from server - would you like to add the server anyway?", MB_ICONQUESTION | MB_YESNO) == IDNO)
			{
				currentMaster->DeleteServer(theServer);
				return;
			}
		}
	}

	m_theServer = theServer;

	if (m_favouritesStr != NOT_A_FAVOURITE)
	{
		theServer->SetFavourite(m_favouritesStr, true);
		UpdateFilterCombo();
	}


	OnOK();
}


void CAddServerDlg::UpdateFilterCombo()
{
	CMainFrame *mainframe = (CMainFrame *)AfxGetMainWnd();
	CComboBox *mainFavsCombo = mainframe->GetFavouritesCombo();

	if (mainFavsCombo->FindString(-1, m_favouritesStr) == -1)
	{
		m_favouritesStr.MakeLower();
		mainFavsCombo->InsertString(mainFavsCombo->GetCount(), m_favouritesStr);
	}
}