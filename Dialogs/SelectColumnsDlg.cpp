// SelectColumnsDlg.cpp : implementation file
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
#include "SelectColumnsDlg.h"
#include "ServerBrowserView.h"
#include "ServerBrowserDoc.h"

#include "MainFrm.h"

// CSelectColumnsDlg dialog



IMPLEMENT_DYNAMIC(CSelectColumnsDlg, CDialog)
CSelectColumnsDlg::CSelectColumnsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectColumnsDlg::IDD, pParent)
{
}

CSelectColumnsDlg::~CSelectColumnsDlg()
{
}

void CSelectColumnsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NOT_SELECTED, m_available);
	DDX_Control(pDX, IDC_SELECTED, m_selected);
}


BEGIN_MESSAGE_MAP(CSelectColumnsDlg, CDialog)
	ON_BN_CLICKED(IDC_SELECT, OnBnClickedSelect)
	ON_BN_CLICKED(IDC_DESELECT, OnBnClickedDeselect)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_LBN_SELCHANGE(IDC_NOT_SELECTED, OnSelchangeList)
	ON_LBN_SELCHANGE(IDC_SELECTED, OnSelchangeList)
END_MESSAGE_MAP()


BOOL CSelectColumnsDlg::OnInitDialog()
{
	BOOL retVal = CDialog::OnInitDialog();


	CMainFrame *mainframe = (CMainFrame *)AfxGetMainWnd();

	mainframe->GetServerBrowserView()->StoreColumnProperties();
	idList<ColumnDetails*> columns = mainframe->GetDocument()->GetCurrentGame()->GetColumnDetails();
	
	for (int i=0; i<columns.Num(); i++)
	{
		if (columns[i]->hidden)
		{
			int index = m_available.AddString(columns[i]->name.c_str());
			m_available.SetItemDataPtr(index, columns[i]);
		}
		else
		{
			int index = m_selected.AddString(columns[i]->name.c_str());
			m_selected.SetItemDataPtr(index, columns[i]);
		}
	}

	OnSelchangeList();

	return retVal;
}

inline void MoveSelected(CListBox& fromList, CListBox& toList)
{
	int i;
	int items = fromList.GetCount();
	for (i = 0; i<items; i++)
	{
		if (fromList.GetSel(i))
		{
			CString ItemText;
			
			fromList.GetText(i,ItemText);
			
			int newIndex = toList.AddString(ItemText);
			toList.SetItemData(newIndex, fromList.GetItemData(i));
		}
	}

	int firstSelItem = -1;
	for (i = 0; i<fromList.GetCount(); i++)
	{
		if (fromList.GetSel(i))
		{
			if (firstSelItem == -1)
				firstSelItem = i;

			fromList.DeleteString(i);
			i--;
		}
	}

	if (fromList.GetCount() > 0)
		fromList.SetSel(__max(0, __min(firstSelItem,fromList.GetCount()-1)));
}

void CSelectColumnsDlg::OnSelchangeList()
{
	GetDlgItem(IDC_SELECT)->EnableWindow(m_available.GetSelCount());
	GetDlgItem(IDC_DESELECT)->EnableWindow(m_selected.GetSelCount());
}

// CSelectColumnsDlg message handlers

void CSelectColumnsDlg::OnBnClickedSelect()
{
	MoveSelected(m_available, m_selected);
	OnSelchangeList();
}

void CSelectColumnsDlg::OnBnClickedDeselect()
{
	MoveSelected(m_selected, m_available);
	OnSelchangeList();
}

void CSelectColumnsDlg::OnBnClickedOk()
{
	int items = m_available.GetCount();
	for (int i = 0; i<items; i++)
	{
		ColumnDetails* thisColumn = (ColumnDetails*)m_available.GetItemDataPtr(i);
		thisColumn->hidden = true;
	}

	items = m_selected.GetCount();
	for (int i = 0; i<items; i++)
	{
		ColumnDetails* thisColumn = (ColumnDetails*)m_selected.GetItemDataPtr(i);
		if (thisColumn->hidden)
			thisColumn->order = -1;

		thisColumn->hidden = false;
	}

	OnOK();
}
