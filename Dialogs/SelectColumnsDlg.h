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

// CSelectColumnsDlg dialog

class CSelectColumnsDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectColumnsDlg)

public:
	CSelectColumnsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectColumnsDlg();

// Dialog Data
	enum { IDD = IDD_SELECT_COLUMNS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSelect();
	afx_msg void OnBnClickedDeselect();
	afx_msg void OnBnClickedOk();

	BOOL OnInitDialog();
	void OnSelchangeList();
	CListBox m_available;
	CListBox m_selected;
};
