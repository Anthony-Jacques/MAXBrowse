#pragma once

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


class SupportedGame;
class GameServer;

// CFilterView view

class CFilterView : public CTreeView
{
	DECLARE_DYNCREATE(CFilterView)

protected:
	CFilterView();           // protected constructor used by dynamic creation
	virtual ~CFilterView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()


	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
public:
	void OnInitialUpdate();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	BOOL PreCreateWindow(CREATESTRUCT& cs);

	void ConstructFilterTree();
	void RefreshTreeForSelectedServers();
	HTREEITEM GetChildNode(HTREEITEM serverParent, const char *value, HTREEITEM &previousSibling);

	void OnDestroy();
	void StoreTreePosition();
	void RestoreTreePosition();

	void EnsureServerVisible(GameServer *thisServer, HTREEITEM gameRoot);

	SupportedGame *m_displayedGame;
};


