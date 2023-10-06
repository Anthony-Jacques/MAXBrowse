// GameConfigPage.cpp : implementation file
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
#include "GameConfigPopup.h"
#include "SupportedGame.h"
#include "GameServer.h"

// CGameConfigPopup dialog

IMPLEMENT_DYNAMIC(CGameConfigPopup, CDialog)
CGameConfigPopup::CGameConfigPopup(SupportedGame* game, CWnd *pParent)
	: CDialog(CGameConfigPopup::IDD, pParent)
	, m_game(game)
	, m_installDir(_T(""))
	, m_additionalParams(_T(""))
	, m_exeName(_T(""))
	, m_gameName(_T(""))
	, m_version(_T(""))
{
}

CGameConfigPopup::~CGameConfigPopup()
{
}

void CGameConfigPopup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INSTALLDIR, m_installDir);
	DDX_Text(pDX, IDC_ADDITIONAL_PARAMS, m_additionalParams);
	DDX_Text(pDX, IDC_EXENAME, m_exeName);
	DDX_Text(pDX, IDC_VERSION, m_version);
	DDX_Control(pDX, IDC_VERSION, m_versionCtrl);
}


BEGIN_MESSAGE_MAP(CGameConfigPopup, CDialog)
	ON_BN_CLICKED(IDC_BROWSE_INSTALLDIR, OnBnClickedBrowseInstalldir)
END_MESSAGE_MAP()


// CGameConfigPopup message handlers
void CGameConfigPopup::OnBnClickedBrowseInstalldir()
{
	UpdateData(TRUE);
	
	CFileDialog dlg(TRUE, m_exeName, m_exeName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, va("%s Installation|*.exe||", m_gameName), this);
	dlg.m_ofn.lpstrInitialDir = m_installDir;

	if (dlg.DoModal() == IDOK)
	{
		CString fullName = dlg.GetPathName();
		m_exeName = dlg.GetFileName();
		m_installDir = fullName.Left(fullName.GetLength() - m_exeName.GetLength());

		UpdateData(FALSE);

		CEdit *editCtrl = (CEdit *)GetDlgItem(IDC_INSTALLDIR);
		editCtrl->SetSel(m_installDir.GetLength(), m_installDir.GetLength());
	}
}

BOOL CGameConfigPopup::OnInitDialog()
{
	BOOL retVal = CDialog::OnInitDialog();

	PopulateVersionDropdown();

	return retVal;
}

void CGameConfigPopup::OnOK()
{
	UpdateData(TRUE);

	if (m_version.IsEmpty())
	{
		AfxMessageBox("You must select the version for this installation");
		return;
	}

	if (m_exeName.IsEmpty())
	{
		AfxMessageBox("You must enter the name of the executable");
		return;
	}

	if (m_installDir.IsEmpty())
	{
		AfxMessageBox("You must select the directory where the game is installed");
		return;
	}

	CDialog::OnOK();
}

void CGameConfigPopup::PopulateVersionDropdown()
{
	const idList<GameServer*> &servers = m_game->GetMasterServer()->GetServers();

	m_versionCtrl.AddString("<default>");

	int numServers = servers.Num();
	for (int i=0; i<numServers; i++)
	{
		idStr protocol = servers[i]->GetServerInfo().GetString("protocol");
		if (protocol.Length() > 0 && m_versionCtrl.FindString(-1, protocol.c_str()) == -1)
			m_versionCtrl.AddString(protocol.c_str());
	}

	if (m_version.GetLength() > 0)
	{
		m_versionCtrl.SelectString(-1, m_version);
		m_versionCtrl.EnableWindow(FALSE);
	}
}