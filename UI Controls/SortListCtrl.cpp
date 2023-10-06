// SortListCtrl.cpp : implementation file
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
#include "SortListCtrl.h"

#include "SortedListView.h"
#include <math.h>

#ifndef HDF_SORTUP
#define HDF_SORTUP              0x0400
#define HDF_SORTDOWN            0x0200
#endif

// CSortListCtrl

IMPLEMENT_DYNAMIC(CSortListCtrl, CListCtrl)

CSortListCtrl::CSortListCtrl()
{
	m_sortColumn = 0;
	m_reverseSort = true;

	OSVERSIONINFO OSversion;
	OSversion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&OSversion);

	m_supportsThemes =
		(OSversion.dwPlatformId == VER_PLATFORM_WIN32_NT && 
		((OSversion.dwMajorVersion == 5 && OSversion.dwMinorVersion >= 1) ||
		 (OSversion.dwMajorVersion > 5)));
}

CSortListCtrl::~CSortListCtrl()
{
}


BEGIN_MESSAGE_MAP(CSortListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
END_MESSAGE_MAP()



// CSortListCtrl message handlers


void CSortListCtrl::SetColumnType(int col, ListColumnType colType) 
{
	HDITEM headerItem;
	headerItem.mask = HDI_LPARAM;
	GetHeaderCtrl()->GetItem(col, &headerItem);

	headerItem.lParam = colType;

	GetHeaderCtrl()->SetItem(col, &headerItem);
}

ListColumnType CSortListCtrl::GetColumnType(int col) 
{
	HDITEM headerItem;
	headerItem.mask = HDI_LPARAM;

	GetHeaderCtrl()->GetItem(col, &headerItem);

	return (ListColumnType)headerItem.lParam;
}


// code based on the MFC CListCtrl::SortItems but with the EX version of the msg. 
// Shame this isnt this in MFC.
BOOL CSortListCtrl::SortItemsEx(PFNLVCOMPARE pfnCompare, DWORD dwData)
{ 
	ASSERT(::IsWindow(m_hWnd)); 
	ASSERT((GetStyle() & LVS_OWNERDATA)==0); 

	return (BOOL) ::SendMessage(m_hWnd, LVM_SORTITEMSEX, dwData, (LPARAM)pfnCompare); 
}

int CALLBACK CSortListCtrl::CompareCallback(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CSortListCtrl* pListCtrl = (CSortListCtrl*) lParamSort;

	CString strItem1 = pListCtrl->GetItemText((int)lParam1, pListCtrl->GetSortColumn());
	CString strItem2 = pListCtrl->GetItemText((int)lParam2, pListCtrl->GetSortColumn());

	int retVal;
	if (pListCtrl->GetColumnType(pListCtrl->GetSortColumn()) == LV_INT)
	{
		retVal = atoi(strItem1) - atoi(strItem2);
	}
	else if (pListCtrl->GetColumnType(pListCtrl->GetSortColumn()) == LV_FLOAT)
	{
		double diff = atof(strItem1) - atof(strItem2);

		if (diff > 0)
			retVal = (int)ceil(diff);
		else
			retVal = (int)floor(diff);
	}
	else
	{
		retVal = strcmp(strItem2, strItem1);
	}

	if (pListCtrl->GetReverseSort())
		retVal = 0 - retVal;

	return retVal;
}


void CSortListCtrl::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (m_sortColumn == pNMListView->iSubItem)
		m_reverseSort = !m_reverseSort;

	m_sortColumn = pNMListView->iSubItem;

	SortItemsEx(CompareCallback, (DWORD_PTR)this);

	SetSortIcon();

	*pResult = 0;
}

void CSortListCtrl::SortNow()
{
	SortItemsEx(CompareCallback, (DWORD_PTR)this);
	SetSortIcon();
}


void CSortListCtrl::SetSortIcon()
{
	if (!m_supportsThemes)
		return;

	CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
	ASSERT(pHeaderCtrl);

	for (int col = pHeaderCtrl->GetItemCount(); col >= 0; --col)
	{
		HDITEM hdrItem;
		hdrItem.mask = HDI_FORMAT;
		pHeaderCtrl->GetItem(col, &hdrItem);
		if (m_sortColumn != col)
			hdrItem.fmt = (hdrItem.fmt & HDF_JUSTIFYMASK) | HDF_STRING;
		else
			hdrItem.fmt = (hdrItem.fmt & HDF_JUSTIFYMASK) | HDF_STRING | 
							(m_reverseSort ? HDF_SORTUP : HDF_SORTDOWN);

		pHeaderCtrl->SetItem(col, &hdrItem);
	}
}
