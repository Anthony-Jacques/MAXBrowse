// CustomiseFilterDlg.cpp : implementation file
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
#include "CustomiseFilterDlg.h"

#include "MainFrm.h"
#include "FilterView.h"
#include "ServerBrowserDoc.h"
#include ".\customisefilterdlg.h"


// CCustomiseFilterDlg dialog

IMPLEMENT_DYNAMIC(CCustomiseFilterDlg, CDialog)
CCustomiseFilterDlg::CCustomiseFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCustomiseFilterDlg::IDD, pParent)
{
}

CCustomiseFilterDlg::~CCustomiseFilterDlg()
{
}

void CCustomiseFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILTER_TREE, m_filterTree);
}


BEGIN_MESSAGE_MAP(CCustomiseFilterDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
	ON_BN_CLICKED(IDC_DELETE, OnBnClickedDelete)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_FILTER_TREE, OnTvnEndLabelEditFilterTree)
	ON_BN_CLICKED(IDC_EDIT, OnBnClickedEdit)
END_MESSAGE_MAP()


// CCustomiseFilterDlg message handlers

BOOL CCustomiseFilterDlg::OnInitDialog()
{
	BOOL retVal = CDialog::OnInitDialog();

	CMainFrame *mainframe = (CMainFrame *)AfxGetMainWnd();

	CServerBrowserDoc *theDoc = (CServerBrowserDoc *)mainframe->GetFilterView()->GetDocument();

	const idList<idStr*> &filterStructure = theDoc->GetFilterStructure();

	int numLevels = filterStructure.Num();

	HTREEITEM rootNode = m_filterTree.InsertItem("Game");

	HTREEITEM parentNode = rootNode;
	for (int i=0; i<numLevels; i++)
	{
		HTREEITEM childNode = m_filterTree.InsertItem(filterStructure[i]->c_str(), parentNode);
		m_filterTree.Expand(parentNode, TVE_EXPAND);
		parentNode = childNode;
	}

	return retVal;
}

void CCustomiseFilterDlg::OnBnClickedAdd()
{
	HTREEITEM parentNode = m_filterTree.GetSelectedItem();
	HTREEITEM childNode = m_filterTree.GetChildItem(parentNode);
	HTREEITEM newNode = m_filterTree.InsertItem("<somevar>", parentNode);
	m_filterTree.Expand(parentNode, TVE_EXPAND);

	if (childNode)
	{
		MoveTreeItem(childNode, newNode);
		m_filterTree.Expand(newNode, TVE_EXPAND);
	}

	m_filterTree.EditLabel(newNode);
}

void CCustomiseFilterDlg::OnBnClickedDelete()
{
	HTREEITEM thisNode = m_filterTree.GetSelectedItem();
	if (!thisNode || thisNode == m_filterTree.GetRootItem())
		return;

	HTREEITEM childNode = m_filterTree.GetChildItem(thisNode);
	HTREEITEM parentNode = m_filterTree.GetParentItem(thisNode);

	childNode = MoveTreeItem(childNode, parentNode);
	m_filterTree.DeleteItem(thisNode);

	m_filterTree.SelectItem(childNode ? childNode : parentNode);
}

void CCustomiseFilterDlg::OnOK()
{
	CMainFrame *mainframe = (CMainFrame *)AfxGetMainWnd();
	CServerBrowserDoc *theDoc = (CServerBrowserDoc *)mainframe->GetFilterView()->GetDocument();

	idList<idStr*> &filterStructure = theDoc->GetFilterStructure();
	filterStructure.DeleteContents(true);

	HTREEITEM parentItem = m_filterTree.GetRootItem();
	while (parentItem)
	{
		HTREEITEM childItem = m_filterTree.GetChildItem(parentItem);
		if (childItem)
			filterStructure.Append(new idStr(m_filterTree.GetItemText(childItem)));

		parentItem = childItem;
	}

	CDialog::OnOK();
}

HTREEITEM CCustomiseFilterDlg::MoveTreeItem(HTREEITEM hItem, HTREEITEM hItemTo, HTREEITEM hItemPos)
{
	if (hItem == NULL || hItemTo == NULL)
		return NULL;
	if (hItem == hItemTo || hItemTo == m_filterTree.GetParentItem(hItem))
		return hItem;

	// check we're not trying to move to a descendant
	HTREEITEM hItemParent = hItemTo;
	while (hItemParent != TVI_ROOT && 
		(hItemParent = m_filterTree.GetParentItem(hItemParent)) != NULL)
		if (hItemParent == hItem)
			return NULL;

	// copy items to new location, recursively, then delete old hierarchy
	// get text, and other info
	CString sText = m_filterTree.GetItemText(hItem);
	TVINSERTSTRUCT tvis;
	tvis.item.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_STATE;
	tvis.item.hItem = hItem;

	// we don't want to copy selection state etc
	tvis.item.stateMask = (UINT)-1 & ~(TVIS_DROPHILITED | TVIS_SELECTED);
	tvis.item.state = m_filterTree.GetItemState(hItem, tvis.item.stateMask);
	m_filterTree.GetItem(&tvis.item);
	tvis.hParent = hItemTo;
	tvis.hInsertAfter = hItemPos;

	// insert this item
	HTREEITEM hItemNew = m_filterTree.InsertItem(&tvis);
	m_filterTree.SetItemText(hItemNew, sText);

	// now move children to under new item
	HTREEITEM hItemChild = m_filterTree.GetChildItem(hItem);
	while (hItemChild != NULL)
	{
		HTREEITEM hItemNextChild = m_filterTree.GetNextSiblingItem(hItemChild);
		MoveTreeItem(hItemChild, hItemNew);
		hItemChild = hItemNextChild;
	}
	m_filterTree.Expand(hItemNew, TVE_EXPAND);

	m_filterTree.SetItemData(hItem, 0);
	m_filterTree.DeleteItem(hItem);

	return hItemNew;
}
void CCustomiseFilterDlg::OnTvnEndLabelEditFilterTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	if (pTVDispInfo->item.pszText)
		m_filterTree.SetItemText(pTVDispInfo->item.hItem, pTVDispInfo->item.pszText);

	*pResult = 0;
}

void CCustomiseFilterDlg::OnBnClickedEdit()
{
	HTREEITEM thisNode = m_filterTree.GetSelectedItem();
	if (!thisNode || thisNode == m_filterTree.GetRootItem())
		return;

	m_filterTree.EditLabel(thisNode);
}

