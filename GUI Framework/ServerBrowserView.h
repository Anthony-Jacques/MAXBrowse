// ServerBrowserView.h : interface of the CServerBrowserView class
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


#pragma once


#include "SortedListView.h"

class CServerBrowserDoc;
class GameServer;
class SupportedGame;



class CServerBrowserView : public CSortedListView
{
protected: // create from serialization only
	CServerBrowserView();
	DECLARE_DYNCREATE(CServerBrowserView)

// Attributes
public:
	CServerBrowserDoc* GetDocument() const;

// Operations
public:

// Overrides
protected:

// Implementation
public:
	virtual ~CServerBrowserView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CServerBrowserView)
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL SwapRows(UINT row1,UINT row2);
	bool m_Dragging;
	UINT m_DragItem;

public:
	void MoveSelections(int offset);

public:
	void OnInitialUpdate();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	void OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);
	void OnDestroy();

	void DisplayServers();
	void SelectServers(idStr searchText, bool nameOnly, bool caseSensitive);
	void SelectAllServers();

	void RefreshSelected();
	void RefreshVisible();

	void AddToFavourites();
	void RemoveFromFavourites();

	void StoreColumnProperties();

	COLORREF GetFullColour() { return m_fullColour; }
	COLORREF GetFavColour() { return m_favColour; }

	void SetFullColour(COLORREF newVal) { m_fullColour = newVal; Invalidate(); }
	void SetFavColour(COLORREF newVal) { m_favColour = newVal; Invalidate(); }

protected:
	void DisplayColumns();
	void PopulateRow(int item, GameServer *thisServer);
	void RefreshStatusbarCounts();

	void OnTimer(UINT_PTR nIDEvent);

	SupportedGame *m_DisplayedGame;

	int *columnIndex;
	CImageList m_specialImagesList;

	void OnNMCustomDraw(NMHDR *pNMHDR, LRESULT *pResult);
	void DrawIconColumn(LPNMLVCUSTOMDRAW lpLVCustomDraw, LRESULT *pResult, int iconIndex);

	COLORREF m_fullColour;
	COLORREF m_favColour;
};

#ifndef _DEBUG  // debug version in ServerBrowserView.cpp
inline CServerBrowserDoc* CServerBrowserView::GetDocument() const
   { return reinterpret_cast<CServerBrowserDoc*>(m_pDocument); }
#endif

