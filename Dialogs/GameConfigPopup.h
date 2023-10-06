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

#include "afxwin.h"

class SupportedGame;

// CGameConfigPopup dialog

class CGameConfigPopup : public CDialog
{
	DECLARE_DYNAMIC(CGameConfigPopup)

public:
	CGameConfigPopup(SupportedGame *game, CWnd* parent=NULL);
	virtual ~CGameConfigPopup();

// Dialog Data
	enum { IDD = IDD_GAME_CONFIG_POPUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_installDir;
	CString m_additionalParams;
	CString m_exeName;
	CString m_gameName;
	CString m_version;
	SupportedGame *m_game;

	afx_msg void OnBnClickedBrowseInstalldir();
	void PopulateVersionDropdown();
	CComboBox m_versionCtrl;
	BOOL OnInitDialog();
	void OnOK();
};
