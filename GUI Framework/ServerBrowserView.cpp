// ServerBrowserView.cpp : implementation of the CServerBrowserView class
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
#include "ServerBrowserView.h"
#include "MainFrm.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "MasterServer.h"
#include "GameServer.h"

#include "FavouritesDlg.h"


// CServerBrowserView

IMPLEMENT_DYNCREATE(CServerBrowserView, CSortedListView)

BEGIN_MESSAGE_MAP(CServerBrowserView, CSortedListView)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchangedList)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDoubleClick)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomDraw)
END_MESSAGE_MAP()

// CServerBrowserView construction/destruction

CServerBrowserView::CServerBrowserView()
			: CSortedListView()
{
	columnIndex = new int[MAX_COLUMNS];
	m_DisplayedGame = NULL;

	m_specialImagesList.Create(12, 12, ILC_MASK | ILC_COLOR32, 3, 3);

	CBitmap bmp;
	bmp.LoadBitmap(IDB_PADLOCK);
	m_specialImagesList.Add(&bmp, RGB(31,41,59));
	bmp.DeleteObject();

	bmp.LoadBitmap(IDB_PUNKBUSTER);
	m_specialImagesList.Add(&bmp, RGB(31,41,59));
	bmp.DeleteObject();

	bmp.LoadBitmap(IDB_FAVOURITE);
	m_specialImagesList.Add(&bmp, RGB(31,41,59));
	bmp.DeleteObject();

	m_sortColumn = 7; // 7th in table above is ping
	m_reverseSort = false;

	m_fullColour = RGB(255, 0, 0);
	m_favColour = RGB(0, 0, 255);
}

CServerBrowserView::~CServerBrowserView()
{
	delete [] columnIndex;
}



// CServerBrowserView diagnostics

#ifdef _DEBUG
void CServerBrowserView::AssertValid() const
{
	CSortedListView::AssertValid();
}

void CServerBrowserView::Dump(CDumpContext& dc) const
{
	CSortedListView::Dump(dc);
}

CServerBrowserDoc* CServerBrowserView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CServerBrowserDoc)));
	return (CServerBrowserDoc*)m_pDocument;
}
#endif //_DEBUG


void CServerBrowserView::OnInitialUpdate()
{
	CSortedListView::OnInitialUpdate();

	theApp.m_mainFrame->PopulateGameMenu();

	CImageList *imageList = (CImageList *)GetDocument()->GetIPNationalityMap().GetImageList();
	GetListCtrl().SetImageList(imageList, LVSIL_SMALL);
	//GetListCtrl().SetImageList(imageList, LVSIL_NORMAL);

	m_favColour = theApp.GetProfileInt("Settings", "FavouritesColour", m_favColour);
	m_fullColour = theApp.GetProfileInt("Settings", "FullColour", m_fullColour);

	DisplayColumns();
	DisplayServers();
}


void CServerBrowserView::DisplayColumns()
{
	while (GetListCtrl().GetHeaderCtrl()->GetItemCount())
        GetListCtrl().DeleteColumn(0);

	if (!m_DisplayedGame)
		return;

	idList<ColumnDetails*> &columnsRef = m_DisplayedGame->GetColumnDetails();

	int numColumns = columnsRef.Num();
	int visibleColumns = 0;
	for (int i=0; i<numColumns; i++)
	{
		ColumnDetails *thisColumn = columnsRef[i];
		
		if (!thisColumn->hidden)
		{
			int index= GetListCtrl().InsertColumn(visibleColumns++, thisColumn->name.c_str(), LVCFMT_LEFT, thisColumn->width);
			columnIndex[index] = i;
			SetColumnType(index, thisColumn->sortType);
		}
	}

	int *columnOrderArray = new int[visibleColumns];
	for (int i=0; i<visibleColumns; i++)
	{
		int order = columnsRef[columnIndex[i]]->order;

		columnOrderArray[i] = (order < 0) ? visibleColumns : order;
	}

	GetListCtrl().SetColumnOrderArray(visibleColumns, columnOrderArray);

	delete [] columnOrderArray;
}

void CServerBrowserView::StoreColumnProperties()
{
	if (!m_DisplayedGame)
		return;

	idList<ColumnDetails*> &columnsRef = m_DisplayedGame->GetColumnDetails();

	int numColumns = columnsRef.Num();

	numColumns = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	int *columnOrder = new int[numColumns];

	GetListCtrl().GetColumnOrderArray(columnOrder, numColumns);
	for (int i=0; i<numColumns; i++)
	{
		ColumnDetails *thisColumn = columnsRef[columnIndex[i]];
		thisColumn->order = columnOrder[i];
		thisColumn->width = GetListCtrl().GetColumnWidth(i);
	}

	delete [] columnOrder;

	m_DisplayedGame->SetSortColumn(m_sortColumn);
	m_DisplayedGame->SetReverseSort(m_reverseSort);
}


void CServerBrowserView::OnDestroy() 
{
	StoreColumnProperties();

	theApp.WriteProfileInt("Settings", "FavouritesColour", m_favColour);
	theApp.WriteProfileInt("Settings", "FullColour", m_fullColour);

	CSortedListView::OnDestroy();
}





// CServerBrowserView message handlers


void CServerBrowserView::PopulateRow(int item, GameServer *thisServer)
{
	idList<ColumnDetails*> &columnsRef = m_DisplayedGame->GetColumnDetails();

	for (int col=0; col<GetListCtrl().GetHeaderCtrl()->GetItemCount(); col++)
	{
		idStr thisKey = columnsRef[columnIndex[col]]->variable;
		idStr thisValue = thisServer->GetServerInfo().GetString(thisKey.c_str());
		GetListCtrl().SetItemText(item, col, thisValue.c_str());

		if (thisKey.ICmp("nationality") == 0)
		{
			int icon = GetDocument()->GetIPNationalityMap().GetFlagImageID(thisValue.c_str());
			GetListCtrl().SetItem(item, col, LVIF_IMAGE, thisValue.c_str(), icon, 0, 0, (LPARAM)thisServer);
		}
	}
}

void CServerBrowserView::DisplayServers()
{
	GetListCtrl().SetRedraw(FALSE);

	GetListCtrl().DeleteAllItems();

	if (!m_DisplayedGame)
	{
		GetListCtrl().SetRedraw(TRUE);
		return;
	}

	const idList<GameServer*> &servers = GetDocument()->GetFilteredServers();
	int numServers = servers.Num();

	int players = 0;
	int bots = 0;

	int item = 0;
	for (int i=0; i<numServers; i++)
	{
		item = GetListCtrl().InsertItem(item, "", -1);
		PopulateRow(item, servers[i]);

		GetListCtrl().SetItemData(item, (DWORD_PTR)servers[i]);

		players += servers[i]->GetNumPlayers();
		bots += servers[i]->GetNumBots();
		item++;
	}

	SortNow();

	GetListCtrl().SetRedraw(TRUE);

	if (AfxGetMainWnd())
		((CMainFrame*)AfxGetMainWnd())->UpdateStatusBar(numServers, players, bots);
}

void CServerBrowserView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if (lHint == VIEW_FILTER_CHANGED || lHint == SERVERS_REFRESHED || lHint == VIEW_FILTER_TREE_CHANGED)
	{
		DisplayServers();
	}
	else if (lHint == VIEW_GAME_CHANGED)
	{
		StoreColumnProperties();
		m_DisplayedGame = GetDocument()->GetCurrentGame();
		if (m_DisplayedGame)
		{
			m_sortColumn = m_DisplayedGame->GetSortColumn();
			m_reverseSort = m_DisplayedGame->GetReverseSort();
		}
		DisplayColumns();
		DisplayServers();
	}
	else if (lHint == SELECTED_SERVERS_REFRESHED)
	{
		RefreshStatusbarCounts();
	}
	else if (lHint == VIEW_COLUMNS_CHANGED)
	{
		DisplayColumns();
		DisplayServers();
	}
}


void CServerBrowserView::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMLV = (NM_LISTVIEW*)pNMHDR;
	
	if (!(pNMLV->uOldState & LVIS_SELECTED) && (pNMLV->uNewState & LVIS_SELECTED))
	{
		GameServer *theServer = NULL;

		int selItem = pNMLV->iItem;
		if (selItem >= 0)
			theServer = (GameServer*)GetListCtrl().GetItemData(selItem);

		GetDocument()->SetSelectedServer(theServer);

		SetTimer(1, 5, NULL);
	}
	
	*pResult = 0;
}

void CServerBrowserView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		KillTimer(1);

		GetDocument()->UpdateAllViews(this, VIEW_SELECTED_SERVER_CHANGED);
	}
}

void CServerBrowserView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	DWORD pos = ::GetMessagePos();
	CPoint point(LOWORD(pos), HIWORD(pos));

	CMenu popup;
	popup.LoadMenu( IDR_MAINFRAME );
	popup.GetSubMenu(2)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
}

void CServerBrowserView::OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	theApp.m_mainFrame->SendMessage(WM_COMMAND, MAKELONG(ID_SERVER_CONNECT, BN_CLICKED), 0);
}



void CServerBrowserView::RefreshVisible()
{
	CWaitCursor wait;

	for (int i=0; i<GetListCtrl().GetItemCount(); i++)
	{
		GameServer *thisServer = (GameServer *)GetListCtrl().GetItemData(i);
		GetDocument()->AddServerToQuery(thisServer);
	}		

	GetDocument()->QueryServers();

	GetListCtrl().SetRedraw(FALSE);

	for (int i=0; i<GetListCtrl().GetItemCount(); i++)
	{
		GameServer *thisServer = (GameServer *)GetListCtrl().GetItemData(i);
		PopulateRow(i, thisServer);
	}

	SortNow();

	GetListCtrl().SetRedraw(TRUE);

	RefreshStatusbarCounts();

	GetDocument()->UpdateAllViews(this, SELECTED_SERVERS_REFRESHED);
}

void CServerBrowserView::RefreshSelected()
{
	CWaitCursor wait;

	POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
	while (pos)
	{
		GameServer *thisServer = (GameServer *)GetListCtrl().GetItemData(GetListCtrl().GetNextSelectedItem(pos));
		GetDocument()->AddServerToQuery(thisServer);
	}

	GetDocument()->QueryServers();

	GetListCtrl().SetRedraw(FALSE);

	pos = GetListCtrl().GetFirstSelectedItemPosition();
	while (pos)
	{
		int item = GetListCtrl().GetNextSelectedItem(pos);
		GameServer *thisServer = (GameServer *)GetListCtrl().GetItemData(item);
		PopulateRow(item, thisServer);
	}

	SortNow();

	GetListCtrl().SetRedraw(TRUE);

	GetDocument()->UpdateAllViews(this, SELECTED_SERVERS_REFRESHED);
}

void CServerBrowserView::SelectServers(idStr searchText, bool nameOnly, bool caseSensitive)
{
	int numRows = GetListCtrl().GetItemCount();
	int numColumns = GetListCtrl().GetHeaderCtrl()->GetItemCount();

	int firstSel = -1;

	for (int i=0; i<numRows; i++)
	{
		bool matches = false;
		for (int j=0; j<numColumns && !matches; j++)
		{
			CString itemText = GetListCtrl().GetItemText(i, j);
			idStr itemTextStr = itemText;
			if (itemTextStr.Find(searchText.c_str(), caseSensitive) > -1)
				matches = true;
		}

		if (matches)
		{
			GetListCtrl().SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			if (firstSel == -1)
				firstSel = i;
		}
		else
		{
			GetListCtrl().SetItemState(i, 0, LVIS_SELECTED);
		}
	}

	if (firstSel == -1)
		AfxMessageBox("No matching servers");
	else
	{
		GetListCtrl().EnsureVisible(firstSel, FALSE);
		SetFocus();
	}
}

void CServerBrowserView::SelectAllServers()
{
	SetRedraw(FALSE);
	int numRows = GetListCtrl().GetItemCount();

	for (int i=0; i<numRows; i++)
		GetListCtrl().SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);

	SetRedraw(TRUE);
	Invalidate();
}

void CServerBrowserView::AddToFavourites()
{
	CFavouritesDlg dlg(this);
	if (dlg.DoModal() == IDOK)
	{
		POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
		while (pos)
		{
			GameServer *thisServer = (GameServer *)GetListCtrl().GetItemData(GetListCtrl().GetNextSelectedItem(pos));
			thisServer->SetFavourite(dlg.m_favouritesStr, true);
		}

		dlg.UpdateFilterCombo();
	}
}

void CServerBrowserView::RemoveFromFavourites()
{
	int removedRows = 0;
	POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
	while (pos)
	{
		GameServer *thisServer = (GameServer *)GetListCtrl().GetItemData(GetListCtrl().GetNextSelectedItem(pos));
		thisServer->SetFavourite(GetDocument()->GetFavouritesFilter().c_str(), false);
		removedRows++;
	}

	if (removedRows == GetListCtrl().GetItemCount())
	{
		CComboBox *mainFavsCombo = theApp.m_mainFrame->GetFavouritesCombo();

		int index = mainFavsCombo->GetCurSel();
		if (index != -1)
		{
			mainFavsCombo->DeleteString(index);
			mainFavsCombo->SelectString(-1, FAVOURITES_NO_FILTER);
			GetDocument()->SetFavouritesFilter("");
		}
	}

	GetDocument()->FilterServerList();
	GetDocument()->UpdateAllViews(NULL, VIEW_FILTER_CHANGED);
}

void CServerBrowserView::RefreshStatusbarCounts()
{
	const idList<GameServer*> &servers = GetDocument()->GetFilteredServers();

	int numServers = servers.Num();
	int players = 0;
	int bots = 0;
	for (int i=0; i<numServers; i++)
	{
		players += servers[i]->GetNumPlayers();
		bots += servers[i]->GetNumBots();
	}

	theApp.m_mainFrame->UpdateStatusBar(numServers, players, bots);
}


void CServerBrowserView::OnNMCustomDraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	CSortedListView::OnNMCustomDraw(pNMHDR, pResult);

	LPNMLVCUSTOMDRAW lpLVCustomDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	switch(lpLVCustomDraw->nmcd.dwDrawStage)
	{
	case CDDS_ITEMPREPAINT:
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		{
			GameServer* thisServer = (GameServer*)lpLVCustomDraw->nmcd.lItemlParam;
/*			if (thisServer->GetServerInfo().GetBool(GetDocument()->GetCurrentGame()->GetPasswordedCvar()) && 
				(thisServer->GetPassword().Length() == 0))
			{
				DWORD windowColor = GetSysColor(COLOR_WINDOWTEXT);
				DWORD alternateColor;
				if (windowColor > RGB(128,128,128))
					alternateColor = windowColor - RGB(40,40,40);
				else
					alternateColor = windowColor + RGB(80,80,80);

				lpLVCustomDraw->clrText = alternateColor;
			}
*/
			if (thisServer->GetNumPlayers() == 
				thisServer->GetServerInfo().GetInt(GetDocument()->GetCurrentGame()->GetMaxPlayersCvar()))
			{
				lpLVCustomDraw->clrText = m_fullColour; 
			}
			else if (thisServer->GetFavourites()->GetNumKeyVals() > 0)
			{
				lpLVCustomDraw->clrText = m_favColour;
			}


			if (lpLVCustomDraw->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM))
			{
				idList<ColumnDetails*> &columnsRef = m_DisplayedGame->GetColumnDetails();
				if (lpLVCustomDraw->iSubItem >= 0 && lpLVCustomDraw->iSubItem < columnsRef.Num())
				{
					idStr thisKey = columnsRef[columnIndex[lpLVCustomDraw->iSubItem]]->variable;
					if (thisKey.ICmp(GetDocument()->GetCurrentGame()->GetPasswordedCvar()) == 0)
					// 
					//if (lpLVCustomDraw->iSubItem == 1)
						DrawIconColumn(lpLVCustomDraw, pResult, 0);
					else if //(lpLVCustomDraw->iSubItem == 2)
						(thisKey.ICmp(GetDocument()->GetCurrentGame()->GetPunkbusterCvar()) == 0)
						DrawIconColumn(lpLVCustomDraw, pResult, 1);
				}
			}
	//		if (lpLVCustomDraw->iSubItem == 2)
		}

		break;

	default: 
		break;    
	}

}

void CServerBrowserView::DrawIconColumn(LPNMLVCUSTOMDRAW lpLVCustomDraw, LRESULT *pResult, int iconIndex)
{
	CDC* pDC = CDC::FromHandle(lpLVCustomDraw->nmcd.hdc);
	CRect rcBounds; // = lpLVCustomDraw->nmcd.rc;
	GetListCtrl().GetSubItemRect(lpLVCustomDraw->nmcd.dwItemSpec, lpLVCustomDraw->iSubItem, LVIR_BOUNDS, rcBounds);

	// (lpLVCustomDraw->iStateId & LVIS_SELECTED) ? GetSysColor(COLOR_HIGHLIGHT) : 
	CBrush brush(lpLVCustomDraw->clrTextBk);
	CBrush *oldBrush = pDC->SelectObject(&brush);
	pDC->FillRect(rcBounds, &brush);
	pDC->SelectObject(oldBrush);

	if (rcBounds.Width() > 12)
		rcBounds.left += (rcBounds.Width() - 12) / 2;

	CString label = GetListCtrl().GetItemText(lpLVCustomDraw->nmcd.dwItemSpec, lpLVCustomDraw->iSubItem);
	if (atoi(label))
		m_specialImagesList.Draw(pDC, iconIndex, rcBounds.TopLeft(), ILD_TRANSPARENT);

	*pResult = CDRF_SKIPDEFAULT;
}
