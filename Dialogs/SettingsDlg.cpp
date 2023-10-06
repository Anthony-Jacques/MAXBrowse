// SettingsDlg.cpp : implementation file
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
#include "SettingsDlg.h"


// CSettingsDlg dialog

IMPLEMENT_DYNAMIC(CSettingsDlg, CDialog)

CSettingsDlg::CSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsDlg::IDD, pParent)
	, m_minimiseToSystray(FALSE)
	, m_minimiseOnConnect(FALSE)
	, m_useICMP(FALSE)
	, m_queryCount(0)
	, m_minimiseOnStartup(FALSE)
	, m_closeOnConnect(FALSE)
	, m_shadeAlternateRows(FALSE)
{

}

CSettingsDlg::~CSettingsDlg()
{
}

void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_MINIMISETOSYSTRAY, m_minimiseToSystray);
	DDX_Check(pDX, IDC_MINIMISEON_CONNECT, m_minimiseOnConnect);
	DDX_Check(pDX, IDC_CLOSE_ON_CONNECT, m_closeOnConnect);
	DDX_Check(pDX, IDC_USEICMP, m_useICMP);
	DDX_Slider(pDX, IDC_QUERYCOUNT, m_queryCount);
	DDX_Control(pDX, IDC_QUERYCOUNT, m_queryCountCtrl);
	DDX_Control(pDX, IDC_FAVOURITES_COLOUR, m_favouritesColour);
	DDX_Control(pDX, IDC_FULL_COLOUR, m_fullColour);
	DDX_Check(pDX, IDC_MINIMISEON_STARTUP, m_minimiseOnStartup);
	DDX_Check(pDX, IDC_SHADE_ALTERNATE_ROWS, m_shadeAlternateRows);
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
END_MESSAGE_MAP()


// CSettingsDlg message handlers
BOOL CSettingsDlg::OnInitDialog()
{
	BOOL retVal = CDialog::OnInitDialog();

	UpdateData(FALSE);
	m_queryCountCtrl.SetRange(1, 20, true);

	return retVal;
}