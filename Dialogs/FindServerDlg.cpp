// FindServerDlg.cpp : implementation file
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
#include "FindServerDlg.h"


// CFindServerDlg dialog

IMPLEMENT_DYNAMIC(CFindServerDlg, CDialog)
CFindServerDlg::CFindServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindServerDlg::IDD, pParent)
	, m_nameOnly(FALSE)
	, m_searchText(_T(""))
	, m_caseSensitive(FALSE)
{
}

CFindServerDlg::~CFindServerDlg()
{
}

void CFindServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_NAME_ONLY, m_nameOnly);
	DDX_Text(pDX, IDC_SEARCH_TEXT, m_searchText);
	DDX_Check(pDX, IDC_CASE_SENSITIVE, m_caseSensitive);
}


BEGIN_MESSAGE_MAP(CFindServerDlg, CDialog)
END_MESSAGE_MAP()


// CFindServerDlg message handlers
