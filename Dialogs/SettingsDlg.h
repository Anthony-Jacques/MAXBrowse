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

#include "resource.h"
#include "colourpicker.h"

// CSettingsDlg dialog

class CSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingsDlg)

public:
	CSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingsDlg();

// Dialog Data
	enum { IDD = IDD_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_minimiseToSystray;
	BOOL m_minimiseOnConnect;
	BOOL m_closeOnConnect;
	BOOL m_useICMP;
	int m_queryCount;
	CSliderCtrl m_queryCountCtrl;
	CColourPicker m_favouritesColour;
	CColourPicker m_fullColour;
	BOOL m_minimiseOnStartup;
	BOOL m_shadeAlternateRows;
};
