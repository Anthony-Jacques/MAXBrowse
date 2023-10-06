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

// CGameConfigPage dialog

class CGameConfigPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CGameConfigPage)

public:
	CGameConfigPage(SupportedGame* game);
	virtual ~CGameConfigPage();

// Dialog Data
	enum { IDD = IDD_GAME_CONFIG_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();
	BOOL OnApply();

	DECLARE_MESSAGE_MAP()
public:

	BOOL m_installed;
	SupportedGame* m_game;
	CString m_gameName;

	idDict m_installDirDict;
	idDict m_exeNameDict;
	idDict m_additionalParamDict;

	void OnOK();

	afx_msg void OnBnClickedInstalled();
	void PopulateInstallationsList();
	afx_msg void OnBnClickedAddInstallation();
	afx_msg void OnBnClickedEditInstallation();
	afx_msg void OnBnClickedDeleteInstallation();
	CSortListCtrl m_installations;
	afx_msg void OnLvnItemchangedInstallations(NMHDR *pNMHDR, LRESULT *pResult);
	void RefreshEnabledStates();
	BOOL m_forceNoSMP;
};
