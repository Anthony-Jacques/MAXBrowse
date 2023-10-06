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

#include "GameConfigPage.h"
#include "SupportedGame.h"

// CConfigureGamesDlg

class CConfigureGamesDlg : public CPropertySheet
{
	DECLARE_DYNAMIC(CConfigureGamesDlg)

public:
	CConfigureGamesDlg(const idList<SupportedGame*> &supportedGames, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CConfigureGamesDlg(const idList<SupportedGame*> &supportedGames, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CConfigureGamesDlg();

protected:
	DECLARE_MESSAGE_MAP()

	void InitialisePages(const idList<SupportedGame*> &supportedGames);
};


