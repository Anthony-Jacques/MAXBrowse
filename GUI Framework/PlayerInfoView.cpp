// PlayerInfoView.cpp : implementation file
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
#include "PlayerInfoView.h"
#include "GameServer.h"
#include "PlayerDetails.h"

// CPlayerInfoView

IMPLEMENT_DYNCREATE(CPlayerInfoView, CSortedListView)

CPlayerInfoView::CPlayerInfoView()
{
}

CPlayerInfoView::~CPlayerInfoView()
{
}

BEGIN_MESSAGE_MAP(CPlayerInfoView, CSortedListView)
END_MESSAGE_MAP()


// CPlayerInfoView diagnostics

#ifdef _DEBUG
void CPlayerInfoView::AssertValid() const
{
	CListView::AssertValid();
}

void CPlayerInfoView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG


// CPlayerInfoView message handlers



void CPlayerInfoView::OnInitialUpdate()
{
	CSortedListView::OnInitialUpdate();

	m_sortColumn = 0;
}

void CPlayerInfoView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if (lHint == VIEW_SELECTED_SERVER_CHANGED || lHint == SELECTED_SERVERS_REFRESHED)
	{
		CServerBrowserDoc* theDoc = (CServerBrowserDoc*)GetDocument();
		DisplayServerInfo(theDoc->GetSelectedServer());
	}
	else if (lHint == VIEW_GAME_CHANGED)
	{
		idList<idStr> &playerColumns = ((CServerBrowserDoc*)GetDocument())->GetCurrentGame()->GetPlayerDetails();
		int numColumns = playerColumns.Num();

		while (GetListCtrl().GetHeaderCtrl()->GetItemCount())
			GetListCtrl().DeleteColumn(0);

		CRect rect;
		GetClientRect(rect);

		for (int i=0; i<numColumns; i++)
		{
			GetListCtrl().InsertColumn(i, playerColumns[i].c_str(), LVCFMT_LEFT, (rect.Width() - 32) / numColumns);

			if (playerColumns[i] != "Name" && playerColumns[i] != "Clan")
				SetColumnType(i, LV_INT);
		}

		CServerBrowserDoc* theDoc = (CServerBrowserDoc*)GetDocument();
		DisplayServerInfo(theDoc->GetSelectedServer());
	}
}

void CPlayerInfoView::DisplayServerInfo(GameServer *aServer)
{
	GetListCtrl().SetRedraw(FALSE);
	GetListCtrl().DeleteAllItems();

	if (!aServer)
	{
		GetListCtrl().SetRedraw(TRUE);
		return;
	}

	idList<idStr> &playerColumns = ((CServerBrowserDoc*)GetDocument())->GetCurrentGame()->GetPlayerDetails();
	int numColumns = playerColumns.Num();

	const idList<PlayerDetails*> &players = aServer->GetPlayers();

	int numPlayers = players.Num();

	int item = 0;
	for (int i=0; i<numPlayers; i++)
	{
		const PlayerDetails *thisPlayer = players[i];
		
		item = GetListCtrl().InsertItem(item, thisPlayer->GetProperty(playerColumns[0].c_str()), 0);

		for (int j=1; j<numColumns; j++)
			GetListCtrl().SetItemText(item, j, thisPlayer->GetProperty(playerColumns[j].c_str()));

		item++;
	}

	SortNow();

	GetListCtrl().SetRedraw(TRUE);
}

