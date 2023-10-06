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

// CFindServerDlg dialog

class CFindServerDlg : public CDialog
{
	DECLARE_DYNAMIC(CFindServerDlg)

public:
	CFindServerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFindServerDlg();

// Dialog Data
	enum { IDD = IDD_FINDSERVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_nameOnly;
	CString m_searchText;
	BOOL m_caseSensitive;
};
