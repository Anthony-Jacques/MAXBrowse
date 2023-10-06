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


class GameServer;

// CAddServerDlg dialog

class CAddServerDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddServerDlg)

public:
	CAddServerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddServerDlg();

// Dialog Data
	enum { IDD = IDD_ADD_SERVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	void UpdateFilterCombo();

	CString m_hostname;
	char m_dottedQuad[20];
	int m_port;
	CComboBox m_favourites;
	CString m_favouritesStr;

	GameServer *m_theServer;
};
