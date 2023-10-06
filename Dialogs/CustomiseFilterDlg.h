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

#include "EditableTreeCtrl.h"

// CCustomiseFilterDlg dialog

class CCustomiseFilterDlg : public CDialog
{
	DECLARE_DYNAMIC(CCustomiseFilterDlg)

public:
	CCustomiseFilterDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCustomiseFilterDlg();

// Dialog Data
	enum { IDD = IDD_CUSTOMISE_FILTER };

	BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEditableTreeCtrl m_filterTree;
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnOK();

protected:
	HTREEITEM MoveTreeItem(HTREEITEM hItem, HTREEITEM hItemTo, HTREEITEM hItemPos = TVI_LAST);
public:
	afx_msg void OnTvnEndLabelEditFilterTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedEdit();
};
