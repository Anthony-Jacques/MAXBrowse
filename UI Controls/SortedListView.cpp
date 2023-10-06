// ServerBrowserView.cpp : implementation of the CSortedListView class
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
#include "SortedListView.h"

#include <math.h>

#ifndef HDF_SORTUP
#define HDF_SORTUP              0x0400
#define HDF_SORTDOWN            0x0200
#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSortedListView

IMPLEMENT_DYNCREATE(CSortedListView, CListView)

BEGIN_MESSAGE_MAP(CSortedListView, CListView)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomDraw)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CSortedListView construction/destruction

CSortedListView::CSortedListView()
{
	m_reverseSort = false;
	m_sortColumn = 0;

	OSVERSIONINFO OSversion;
	OSversion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&OSversion);

	m_supportsThemes =
		(OSversion.dwPlatformId == VER_PLATFORM_WIN32_NT && 
		((OSversion.dwMajorVersion == 5 && OSversion.dwMinorVersion >= 1) ||
		 (OSversion.dwMajorVersion > 5)));
}

CSortedListView::~CSortedListView()
{
}

BOOL CSortedListView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style = cs.style | LVS_REPORT | LVS_SHOWSELALWAYS;

	return CListView::PreCreateWindow(cs);
}

void CSortedListView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	GetListCtrl().ModifyStyle(NULL, (LVS_REPORT | LVS_SHOWSELALWAYS), 0); 
	GetListCtrl().SetExtendedStyle(LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT);

	m_shadeAlternateRows = theApp.GetProfileInt("Settings", "ShadeAlternateRows", true);
}

void CSortedListView::OnDestroy()
{
	CListView::OnDestroy();

	theApp.WriteProfileInt("Settings", "ShadeAlternateRows", m_shadeAlternateRows);
}


// CSortedListView diagnostics

#ifdef _DEBUG
void CSortedListView::AssertValid() const
{
	CListView::AssertValid();
}

void CSortedListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// Code used for a reasonably generic ClistCtrl sort routine

void CSortedListView::SetColumnType(int col, ListColumnType colType) 
{
	HDITEM headerItem;
	headerItem.mask = HDI_LPARAM;
	GetListCtrl().GetHeaderCtrl()->GetItem(col, &headerItem);

	headerItem.lParam = colType;

	GetListCtrl().GetHeaderCtrl()->SetItem(col, &headerItem);
}

ListColumnType CSortedListView::GetColumnType(int col) 
{
	HDITEM headerItem;
	headerItem.mask = HDI_LPARAM;

	GetListCtrl().GetHeaderCtrl()->GetItem(col, &headerItem);

	return (ListColumnType)headerItem.lParam;
}


// code based on the MFC CListCtrl::SortItems but with the EX version of the msg. 
// Shame this isnt this in MFC.
BOOL CSortedListView::SortItemsEx(PFNLVCOMPARE pfnCompare, DWORD dwData)
{ 
	ASSERT(::IsWindow(m_hWnd)); 
	ASSERT((GetStyle() & LVS_OWNERDATA)==0); 

	return (BOOL) ::SendMessage(GetListCtrl().m_hWnd, LVM_SORTITEMSEX, dwData, (LPARAM)pfnCompare); 
}

int CALLBACK CSortedListView::CompareCallback(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CSortedListView* pListCtrl = (CSortedListView*)lParamSort;

	CString strItem1 = pListCtrl->GetListCtrl().GetItemText((int)lParam1, pListCtrl->GetSortColumn());
	CString strItem2 = pListCtrl->GetListCtrl().GetItemText((int)lParam2, pListCtrl->GetSortColumn());

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
	{
		retVal = 0 - retVal;
	}

	return retVal;
}


void CSortedListView::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (m_sortColumn == pNMListView->iSubItem)
		m_reverseSort = !m_reverseSort;

	m_sortColumn = pNMListView->iSubItem;

	SortItemsEx(CompareCallback, (DWORD_PTR)this);

	SetSortIcon();

	*pResult = 0;
}

void CSortedListView::SortNow()
{
	SortItemsEx(CompareCallback, (DWORD_PTR)this);
	SetSortIcon();
}

void CSortedListView::SetSortIcon()
{
	if (!m_supportsThemes)
		return;

	CHeaderCtrl* pHeaderCtrl = GetListCtrl().GetHeaderCtrl();
	ASSERT(pHeaderCtrl);

	for (int col = pHeaderCtrl->GetItemCount(); col >= 0; --col)
	{
		HDITEM hdrItem;
		hdrItem.mask = HDI_FORMAT;
		pHeaderCtrl->GetItem(col, &hdrItem);
		if (m_sortColumn != col)
		{
			hdrItem.fmt = (hdrItem.fmt & HDF_JUSTIFYMASK) | HDF_STRING;
		}
		else
		{
			hdrItem.fmt = (hdrItem.fmt & HDF_JUSTIFYMASK) | HDF_STRING | (m_reverseSort ? HDF_SORTUP : HDF_SORTDOWN);
		}

		pHeaderCtrl->SetItem(col, &hdrItem);
	}
}


void CSortedListView::OnNMCustomDraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW lpLVCustomDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	switch(lpLVCustomDraw->nmcd.dwDrawStage)
	{
	case CDDS_ITEMPREPAINT:
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		if (m_shadeAlternateRows && lpLVCustomDraw->nmcd.dwItemSpec % 2)
		{
			DWORD windowColor = GetSysColor(COLOR_WINDOW);
			DWORD alternateColor;

			if (windowColor > RGB(128,128,128))
				alternateColor = windowColor - RGB(20,20,20);
			else
				alternateColor = windowColor + RGB(20,20,20);

			lpLVCustomDraw->clrText = CLR_DEFAULT;
			lpLVCustomDraw->clrTextBk = alternateColor;
		}
		else 
		{
			lpLVCustomDraw->clrText = CLR_DEFAULT;
			lpLVCustomDraw->clrTextBk = GetSysColor(COLOR_WINDOW);
		}
		break;

	default: 
		break;    
	}

	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;
	*pResult |= CDRF_NOTIFYITEMDRAW;
	*pResult |= CDRF_NOTIFYSUBITEMDRAW;
}

void CSortedListView::ShadeAlternateRows(bool newVal) 
{ 
	m_shadeAlternateRows = newVal; 
	Invalidate();
}