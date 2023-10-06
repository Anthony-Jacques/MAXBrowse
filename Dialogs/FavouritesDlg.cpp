// FavouritesDlg.cpp : implementation file
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
#include "FavouritesDlg.h"
#include "MainFrm.h"


// CFavouritesDlg dialog

IMPLEMENT_DYNAMIC(CFavouritesDlg, CDialog)
CFavouritesDlg::CFavouritesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFavouritesDlg::IDD, pParent)

{
}

CFavouritesDlg::~CFavouritesDlg()
{
}

void CFavouritesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FAVOURITES_COMBO, m_favourites);
	DDX_Text(pDX, IDC_FAVOURITES_COMBO, m_favouritesStr);
}


BEGIN_MESSAGE_MAP(CFavouritesDlg, CDialog)
END_MESSAGE_MAP()


// CFavouritesDlg message handlers


BOOL CFavouritesDlg::OnInitDialog()
{
	BOOL retVal = CDialog::OnInitDialog();

	CMainFrame *mainframe = (CMainFrame *)AfxGetMainWnd();
	CComboBox *mainFavsCombo = mainframe->GetFavouritesCombo();

	int countFavs = mainFavsCombo->GetCount();
	for (int i=0; i<countFavs; i++)
	{
		CString name;
		mainFavsCombo->GetLBText(i, name);

		if (name != FAVOURITES_NO_FILTER)
			m_favourites.AddString(name);
	}

	return retVal;
}

void CFavouritesDlg::UpdateFilterCombo()
{
	CMainFrame *mainframe = (CMainFrame *)AfxGetMainWnd();
	CComboBox *mainFavsCombo = mainframe->GetFavouritesCombo();

	m_favouritesStr.MakeLower();

	if (mainFavsCombo->FindStringExact(-1, m_favouritesStr) == -1)
		mainFavsCombo->InsertString(mainFavsCombo->GetCount(), m_favouritesStr);
}