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


// CFavouritesDlg dialog

class CFavouritesDlg : public CDialog
{
	DECLARE_DYNAMIC(CFavouritesDlg)

public:
	CFavouritesDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFavouritesDlg();

// Dialog Data
	enum { IDD = IDD_FAVOURITES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_favourites;
	CString m_favouritesStr;

	BOOL OnInitDialog();
	void UpdateFilterCombo();
};
