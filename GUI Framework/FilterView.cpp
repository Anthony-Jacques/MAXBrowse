// FilterView.cpp : implementation file
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
#include "FilterView.h"
#include "GameServer.h"
#include "MainFrm.h"

// CFilterView

IMPLEMENT_DYNCREATE(CFilterView, CTreeView)

CFilterView::CFilterView()
{
	m_displayedGame = NULL;
}

CFilterView::~CFilterView()
{
}

BOOL CFilterView::PreCreateWindow(CREATESTRUCT& cs)
{
	// Modify the Window class or styles here to get the tree looking right
	cs.style |= TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS;

	return CTreeView::PreCreateWindow(cs);
}

BEGIN_MESSAGE_MAP(CFilterView, CTreeView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CFilterView diagnostics

#ifdef _DEBUG
void CFilterView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CFilterView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG


// CFilterView message handlers
void CFilterView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	m_displayedGame = ((CServerBrowserDoc*)GetDocument())->GetCurrentGame();

	ConstructFilterTree();
}

HTREEITEM CFilterView::GetChildNode(HTREEITEM serverParent, const char *value, HTREEITEM &previousSibling)
{
	previousSibling = TVI_FIRST;

	HTREEITEM childNode = GetTreeCtrl().GetChildItem(serverParent);
	while (childNode)
	{
		int comparison = GetTreeCtrl().GetItemText(childNode).CompareNoCase(value);
		if (comparison == 0)
			return childNode;
		else if (comparison > 0)
			return NULL;

		previousSibling = childNode;

		childNode = GetTreeCtrl().GetNextSiblingItem(childNode);
	}

	return NULL;
}

void CFilterView::EnsureServerVisible(GameServer *thisServer, HTREEITEM gameRoot)
{
	CMainFrame *mainFrame = (CMainFrame *)AfxGetMainWnd();
	CComboBox *protocolCombo = mainFrame ? mainFrame->GetProtocolCombo() : NULL;
	CComboBox *favouritesCombo = mainFrame ? mainFrame->GetFavouritesCombo() : NULL;

	const idList<idStr*> filterStructure = ((CServerBrowserDoc*)GetDocument())->GetFilterStructure();
	int treeDepth = filterStructure.Num();

	HTREEITEM serverParent = gameRoot;
	for (int j=0; j<treeDepth; j++)
	{
		const char *value = thisServer->GetServerInfo().GetString(filterStructure[j]->c_str());
		if (idStr::Length(value) == 0)
			value = "<Unknown>";

		HTREEITEM insertAfter = TVI_FIRST;
		HTREEITEM thisNode = GetChildNode(serverParent, value, insertAfter);
		if (!thisNode)
		{
			thisNode = GetTreeCtrl().InsertItem(value, serverParent, insertAfter);
			GetTreeCtrl().SetItemData(thisNode, (DWORD_PTR)filterStructure[j]);
		}

		if (protocolCombo)
		{
			idStr protocol = thisServer->GetServerInfo().GetString("protocol");
			if (protocol.Length() > 0 && protocolCombo->FindString(-1, protocol.c_str()) == -1)
				protocolCombo->AddString(protocol.c_str());
		}

		if (favouritesCombo)
		{
			const idDict *theseFavs = thisServer->GetFavourites();

			int numFavGroups = theseFavs->GetNumKeyVals();
			for (int j=0; j<numFavGroups; j++)
			{
				const idKeyValue *thisPair = theseFavs->GetKeyVal(j);
				
				if (idStr::ICmp(thisPair->GetValue().c_str(), "1") == 0)
				{
					idStr groupName = thisPair->GetKey();
					if (favouritesCombo->FindString(-1, groupName.c_str()) == -1)
						favouritesCombo->AddString(groupName.c_str());
				}
			}
		}

		serverParent = thisNode;
	}
}

void CFilterView::ConstructFilterTree()
{
	GetTreeCtrl().SetRedraw(FALSE);
	GetTreeCtrl().DeleteAllItems();

	CServerBrowserDoc* theDoc = (CServerBrowserDoc*)GetDocument();

	if (!theDoc->GetCurrentGame())
	{
		GetTreeCtrl().SetRedraw(TRUE);
		return;
	}

	HTREEITEM gameRoot = GetTreeCtrl().InsertItem(theDoc->GetCurrentGame()->GetName().c_str());

	CMainFrame *mainFrame = (CMainFrame *)AfxGetMainWnd();
	CComboBox *protocolCombo = mainFrame ? mainFrame->GetProtocolCombo() : NULL;
	CComboBox *favouritesCombo = mainFrame ? mainFrame->GetFavouritesCombo() : NULL;

	if (protocolCombo)
	{
		protocolCombo->ResetContent();
		protocolCombo->InsertString(0, "<All versions>");
		protocolCombo->SelectString(-1, "<All versions>");
	}

	if (favouritesCombo)
	{
		favouritesCombo->ResetContent();
		favouritesCombo->InsertString(0, FAVOURITES_NO_FILTER);
		favouritesCombo->SelectString(-1, FAVOURITES_NO_FILTER);
	}

	const idList<GameServer*> &servers = theDoc->GetCurrentGame()->GetMasterServer()->GetServers();
	int numServers = servers.Num();

	for (int i=0; i<numServers; i++)
		EnsureServerVisible(servers[i], gameRoot);

	if (protocolCombo && theDoc->GetProtocolFilter().Length() > 0)
	{
		if (protocolCombo->SelectString(-1, theDoc->GetProtocolFilter().c_str())  == CB_ERR)
			theDoc->SetProtocolFilter("");
	}

	if (favouritesCombo && theDoc->GetFavouritesFilter().Length() > 0)
	{
		if (favouritesCombo->SelectString(-1, theDoc->GetFavouritesFilter().c_str()) == CB_ERR)
			theDoc->SetFavouritesFilter("");
	}

	GetTreeCtrl().Expand(gameRoot, TVE_EXPAND);

	RestoreTreePosition();

	GetTreeCtrl().SetRedraw(TRUE);
}


void CFilterView::RefreshTreeForSelectedServers()
{
	CServerBrowserDoc* theDoc = (CServerBrowserDoc*)GetDocument();
	if (!theDoc->GetCurrentGame())
		return;

	GetTreeCtrl().SetRedraw(FALSE);

	const idList<GameServer*> &servers = theDoc->GetCurrentGame()->GetMasterServer()->GetServers();

	HTREEITEM gameRoot = GetTreeCtrl().GetRootItem();

	int numServers = servers.Num();
	for (int i=0; i<numServers; i++)
		EnsureServerVisible(servers[i], gameRoot);

	GetTreeCtrl().SetRedraw(TRUE);
}

void CFilterView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if (lHint == VIEW_FILTER_TREE_CHANGED)
	{
		ConstructFilterTree();
	}
	else if (lHint == SERVERS_REFRESHED || lHint == SELECTED_SERVERS_REFRESHED)
	{
		RefreshTreeForSelectedServers();
	}
	else if (lHint == VIEW_GAME_CHANGED)
	{
		StoreTreePosition();

		m_displayedGame = ((CServerBrowserDoc*)GetDocument())->GetCurrentGame();

		ConstructFilterTree();
	}
}

void CFilterView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	if ((pNMTreeView->action == 1) || (pNMTreeView->action == 2))
	{
		HTREEITEM selItem = GetTreeCtrl().GetSelectedItem();

		idDict filter;
		while (selItem && GetTreeCtrl().GetItemData(selItem))
		{
			CString value = GetTreeCtrl().GetItemText(selItem);
			idStr *key = (idStr *)GetTreeCtrl().GetItemData(selItem);

			if (value == "<Unknown>")
				value = "";
			filter.Set(key->c_str(), value);

			selItem = GetTreeCtrl().GetParentItem(selItem);
		}

		CServerBrowserDoc* theDoc = (CServerBrowserDoc*)GetDocument();
		theDoc->SetFilter(filter);
		theDoc->FilterServerList();

		theDoc->UpdateAllViews(this, VIEW_FILTER_CHANGED);
	}
}
void CFilterView::OnDestroy() 
{
	StoreTreePosition();

	CTreeView::OnDestroy();
}


void CFilterView::StoreTreePosition()
{
	HTREEITEM selItem = GetTreeCtrl().GetSelectedItem();

	idStr currentPath = "";
	while (selItem)
	{
		CString value = GetTreeCtrl().GetItemText(selItem);
		idStr *key = (idStr *)GetTreeCtrl().GetItemData(selItem);

		if (currentPath.Length() == 0)
			currentPath = (const char*)value;
		else
			currentPath = va("%s\\%s", value, currentPath.c_str());

		selItem = GetTreeCtrl().GetParentItem(selItem);
	}

	if (currentPath.Length() > 0)
		m_displayedGame->SetCurrentFilterPos(currentPath);
}

void CFilterView::RestoreTreePosition()
{
	if (!m_displayedGame)
		return;

	char buffer[1024];
	idStr curPos = m_displayedGame->GetCurrentFilterPos();

	strcpy(buffer, curPos.c_str());

	HTREEITEM parentNode = TVI_ROOT;
	char *thisNodeName = buffer;
	while (*thisNodeName)
	{
		char *nextNodeName = strstr(thisNodeName, "\\");
		if (nextNodeName)
		{
			*nextNodeName = 0;
			nextNodeName++;
		}
		else 
			nextNodeName = thisNodeName + strlen(thisNodeName);

		HTREEITEM unused;
		parentNode = GetChildNode(parentNode, thisNodeName, unused);
		if (parentNode)
		{
			if (*nextNodeName)
				GetTreeCtrl().Expand(parentNode, TVE_EXPAND);

			GetTreeCtrl().SelectItem(parentNode);
		}

		thisNodeName = nextNodeName;
	}

	LRESULT res = 0;
	NM_TREEVIEW hdr;
	hdr.action = 1;
	OnSelchanged((NMHDR*)&hdr, &res);
}