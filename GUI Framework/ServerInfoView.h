// ServerBrowserView.h : interface of the CServerInfoView class
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

class CServerInfoView : public CSortedListView
{
protected: // create from serialization only
	CServerInfoView();
	DECLARE_DYNCREATE(CServerInfoView)

// Attributes
public:
	CServerBrowserDoc* GetDocument() const;

// Operations
public:

// Overrides
protected:

// Implementation
public:
	virtual ~CServerInfoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CServerInfoView)
	afx_msg void OnEditCopyServerInfo();
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

	void OnInitialUpdate();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

	void DisplayServerInfo(GameServer *aServer);
	
};

#ifndef _DEBUG  // debug version in ServerBrowserView.cpp
inline CServerBrowserDoc* CServerInfoView::GetDocument() const
   { return reinterpret_cast<CServerBrowserDoc*>(m_pDocument); }
#endif

