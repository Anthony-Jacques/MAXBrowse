// ServerBrowserView.h : interface of the CSortedListView class
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


enum ListColumnType { LV_TEXT, LV_INT, LV_FLOAT };

class CServerBrowserDoc;

class CSortedListView : public CListView
{
protected: // create from serialization only
	CSortedListView();
	DECLARE_DYNCREATE(CSortedListView)

// Attributes
public:
// Operations
public:

// Overrides
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void OnInitialUpdate();

protected:

// Implementation
public:
	virtual ~CSortedListView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSortedListView)
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg /*virtual*/ void OnNMCustomDraw(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	int m_sortColumn;
	bool m_reverseSort;
	bool m_supportsThemes;
	bool m_shadeAlternateRows;

public:
	// sorting operations
	BOOL SortItemsEx(PFNLVCOMPARE pfnCompare, DWORD dwData);
	static int CALLBACK CompareCallback(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	void SetColumnType(int col, ListColumnType colType);
	ListColumnType GetColumnType(int col);

	// sorting accessors
	int GetSortColumn() { return m_sortColumn; }
	void SetSortColumn(int col) { m_sortColumn = col; SetSortIcon(); }
	bool GetReverseSort() { return m_reverseSort; }
	void SortNow();

	void SetSortIcon();
	afx_msg void OnDestroy();

	bool ShadeAlternateRows() { return m_shadeAlternateRows; }
	void ShadeAlternateRows(bool newVal);
};

