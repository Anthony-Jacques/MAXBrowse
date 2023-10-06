#pragma once

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

#include "afxcmn.h"

#include "SortListCtrl.h"

class SupportedGame;

// CServerDetailsDlg dialog

class GameServer;

class CServerDetailsDlg : public CDialog
{
	DECLARE_DYNAMIC(CServerDetailsDlg)

public:
	CServerDetailsDlg(GameServer *theServer, CWnd* pParent = NULL);   // standard constructor
	virtual ~CServerDetailsDlg();

// Dialog Data
	enum { IDD = IDD_SERVER_DETAILS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void DisplayServer();

	DECLARE_MESSAGE_MAP()
public:
	BOOL OnInitDialog();
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnBnClickedConnect();
	void OnGetMinMaxInfo(MINMAXINFO* aInfo);
	void MoveObject(int aDlgId, int aDeltaTop, int aDeltaLeft, int aDeltaWidth, int aDeltaHeight);
	void OnSize(UINT nType, int cx, int cy);

	CString m_ip;
	CString m_ping;

	GameServer *m_server;
	SupportedGame *m_theGame;

	CSortListCtrl m_serverInfo;
	afx_msg void OnBnClickedOk();
	CSortListCtrl m_players;
	CString m_name;
	CString m_playerCount;
	CString m_punkbuster;
	CString m_passworded;
	CString m_gametype;
	CString m_map;
	CString m_mod;

	bool m_FirstTime;
	int m_LastCx;
	int m_LastCy;
};
