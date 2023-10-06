// ServerBrowserView.cpp : implementation of the CServerInfoView class
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
#include "ServerBrowserDoc.h"
#include "ServerInfoView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "GameServer.h"

// CServerInfoView

IMPLEMENT_DYNCREATE(CServerInfoView, CSortedListView)

BEGIN_MESSAGE_MAP(CServerInfoView, CSortedListView)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_COMMAND(WM_COPY,OnEditCopyServerInfo)
END_MESSAGE_MAP()

// CServerInfoView construction/destruction

CServerInfoView::CServerInfoView()
{
	// TODO: add construction code here

}

CServerInfoView::~CServerInfoView()
{
}


// CServerInfoView diagnostics

#ifdef _DEBUG
void CServerInfoView::AssertValid() const
{
	CSortedListView::AssertValid();
}

void CServerInfoView::Dump(CDumpContext& dc) const
{
	CSortedListView::Dump(dc);
}

CServerBrowserDoc* CServerInfoView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CServerBrowserDoc)));
	return (CServerBrowserDoc*)m_pDocument;
}
#endif //_DEBUG


void CServerInfoView::OnInitialUpdate()
{
	CSortedListView::OnInitialUpdate();

	int filterTreeWidth = theApp.GetProfileInt("Positions", "FilterTreeWidth", 200);

	GetListCtrl().InsertColumn(0, "Key", LVCFMT_LEFT, (filterTreeWidth - 32) / 2);
	GetListCtrl().InsertColumn(1, "Value", LVCFMT_LEFT, (filterTreeWidth - 32) / 2);

	m_sortColumn = 0;
}

void CServerInfoView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if (lHint == VIEW_SELECTED_SERVER_CHANGED || lHint == SELECTED_SERVERS_REFRESHED)
	{
		CServerBrowserDoc* theDoc = (CServerBrowserDoc*)GetDocument();
		DisplayServerInfo(theDoc->GetSelectedServer());
	}
}

void CServerInfoView::DisplayServerInfo(GameServer *aServer)
{
	GetListCtrl().SetRedraw(FALSE);
	GetListCtrl().DeleteAllItems();

	if (!aServer)
	{
		GetListCtrl().SetRedraw(TRUE);
		return;
	}

	const idDict &serverInfo = aServer->GetServerInfo();

	int numKeys = serverInfo.GetNumKeyVals();
	int item = 0;
	for (int j=0; j<numKeys; j++)
	{
		const idKeyValue *thisPair = serverInfo.GetKeyVal(j);
		
		item = GetListCtrl().InsertItem(item, thisPair->GetKey().c_str(), 0);
		GetListCtrl().SetItemText(item, 1, thisPair->GetValue().c_str());
		item++;
	}

	SortNow();

	GetListCtrl().SetRedraw(TRUE);
}


void CServerInfoView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	DWORD pos = ::GetMessagePos();
	CPoint point(LOWORD(pos), HIWORD(pos));

	CMenu popup;
	popup.CreatePopupMenu();
	popup.AppendMenu(MF_STRING, WM_COPY, "Copy");

	popup.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
}


void CServerInfoView::OnEditCopyServerInfo()
{
	CString copyText;

	int numColumns = GetListCtrl().GetHeaderCtrl()->GetItemCount();

	POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
	while (pos)
	{
		int row = GetListCtrl().GetNextSelectedItem(pos);
		
		copyText += GetListCtrl().GetItemText(row, 0);
		copyText += " = ";
		copyText += GetListCtrl().GetItemText(row, 1);

		if (pos)
			copyText += ", ";
	}

	theApp.m_mainFrame->CopyText(copyText);
}
