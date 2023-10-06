// ConfigureGamesDlg.cpp : implementation file
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
#include "ConfigureGamesDlg.h"


// CConfigureGamesDlg

IMPLEMENT_DYNAMIC(CConfigureGamesDlg, CPropertySheet)
CConfigureGamesDlg::CConfigureGamesDlg(const idList<SupportedGame*> &supportedGames, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	InitialisePages(supportedGames);
}

CConfigureGamesDlg::CConfigureGamesDlg(const idList<SupportedGame*> &supportedGames, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	InitialisePages(supportedGames);
}

CConfigureGamesDlg::~CConfigureGamesDlg()
{
	while (m_pages.GetCount())
	{
		CPropertyPage *thisPage = GetPage(0);
		RemovePage(0);
		delete thisPage;
	}
}


BEGIN_MESSAGE_MAP(CConfigureGamesDlg, CPropertySheet)
END_MESSAGE_MAP()


// CConfigureGamesDlg message handlers



void CConfigureGamesDlg::InitialisePages(const idList<SupportedGame*> &supportedGames)
{
	int numGames = supportedGames.Num();

	for (int i=0; i<numGames; i++)
	{
		CGameConfigPage *thisPage = new CGameConfigPage(supportedGames[i]);

		AddPage(thisPage);
	}
}
