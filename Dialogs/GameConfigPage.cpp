// GameConfigPage.cpp : implementation file
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
#include "GameConfigPage.h"
#include "SupportedGame.h"
#include "GameConfigPopup.h"
#include ".\gameconfigpage.h"

// CGameConfigPage dialog

IMPLEMENT_DYNAMIC(CGameConfigPage, CPropertyPage)
CGameConfigPage::CGameConfigPage(SupportedGame* game)
	: CPropertyPage(CGameConfigPage::IDD)
	, m_installed(FALSE)
	, m_game(game)
	, m_forceNoSMP(FALSE)
{
	m_gameName = m_game->GetName().c_str();
	m_psp.pszTitle = m_gameName;
	m_psp.dwFlags |= PSP_USETITLE;
}

CGameConfigPage::~CGameConfigPage()
{
}

void CGameConfigPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_INSTALLED, m_installed);
	DDX_Control(pDX, IDC_INSTALLATIONS, m_installations);
	DDX_Check(pDX, IDC_FORCENOSMP, m_forceNoSMP);
}


BEGIN_MESSAGE_MAP(CGameConfigPage, CPropertyPage)
	ON_BN_CLICKED(IDC_INSTALLED, OnBnClickedInstalled)
	ON_BN_CLICKED(IDC_ADD_INSTALLATION, OnBnClickedAddInstallation)
	ON_BN_CLICKED(IDC_EDIT_INSTALLATION, OnBnClickedEditInstallation)
	ON_BN_CLICKED(IDC_DELETE_INSTALLATION, OnBnClickedDeleteInstallation)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_INSTALLATIONS, OnLvnItemchangedInstallations)
END_MESSAGE_MAP()


// CGameConfigPage message handlers

BOOL CGameConfigPage::OnInitDialog()
{
	BOOL retVal = CPropertyPage::OnInitDialog();

	m_installed = m_game->IsInstalled();
	m_forceNoSMP = m_game->GetForceNoSMP();

	UpdateData(FALSE);

	m_installDirDict = m_game->GetInstallDirDict();
	m_exeNameDict = m_game->GetExeNameDict();
	m_additionalParamDict = m_game->GetAdditionalParamDict();

	CRect rect;
	m_installations.GetWindowRect(rect);

	int dirWidth = (rect.Width() - 32) / 2;
	int width = (rect.Width() - 32) / 6;
	m_installations.InsertColumn(0, "Version", LVCFMT_LEFT, width);
	m_installations.InsertColumn(1, "Directory", LVCFMT_LEFT, dirWidth);
	m_installations.InsertColumn(2, "Executable", LVCFMT_LEFT, width);
	m_installations.InsertColumn(3, "Parameters", LVCFMT_LEFT, width);
	m_installations.SetExtendedStyle(LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT);

	PopulateInstallationsList();
	RefreshEnabledStates();

	return retVal;
}

void CGameConfigPage::PopulateInstallationsList()
{
	m_installations.DeleteAllItems();

	int numInstalls = m_installDirDict.GetNumKeyVals();
	for (int i=0; i<numInstalls; i++)
	{
		const idKeyValue *keyVal = m_installDirDict.GetKeyVal(i);

		int item = m_installations.InsertItem(i, keyVal->GetKey().c_str());

		m_installations.SetItemText(item, 1, keyVal->GetValue().c_str());
		m_installations.SetItemText(item, 2, m_exeNameDict.GetString(keyVal->GetKey().c_str()));
		m_installations.SetItemText(item, 3, m_additionalParamDict.GetString(keyVal->GetKey().c_str()));
	}
}

void CGameConfigPage::OnOK()
{
	m_installations.DeleteAllItems();

	int numInstalls = m_installDirDict.GetNumKeyVals();
	for (int i=0; i<numInstalls; i++)
	{
		const idKeyValue *keyVal = m_installDirDict.GetKeyVal(i);

		int item = m_installations.InsertItem(i, keyVal->GetKey().c_str());

		m_installations.SetItemText(item, 1, keyVal->GetValue().c_str());
		m_installations.SetItemText(item, 2, m_exeNameDict.GetString(keyVal->GetKey().c_str()));
		m_installations.SetItemText(item, 3, m_additionalParamDict.GetString(keyVal->GetKey().c_str()));
	}
}

void CGameConfigPage::OnBnClickedInstalled()
{
	UpdateData(TRUE);

	SetModified(TRUE);

	RefreshEnabledStates();
}


BOOL CGameConfigPage::OnApply()
{
	BOOL retVal = CPropertyPage::OnApply();

	UpdateData(TRUE);

	// store values into dict

	m_game->SetInstalled(m_installed);
	m_game->SetForceNoSMP(m_forceNoSMP);

	m_game->SetExeNames(m_exeNameDict);
	m_game->SetInstallDirs(m_installDirDict);
	m_game->SetAdditionalParams(m_additionalParamDict);

	m_game->SaveConfigurationSettings();

	return retVal;
}



void CGameConfigPage::OnBnClickedAddInstallation()
{
	CGameConfigPopup dlg(m_game, GetParent());
	dlg.m_gameName = m_gameName;

	if (dlg.DoModal() == IDOK)
	{
		SetModified(TRUE);

		m_installDirDict.Set(dlg.m_version, dlg.m_installDir);
		m_additionalParamDict.Set(dlg.m_version, dlg.m_additionalParams);
		m_exeNameDict.Set(dlg.m_version, dlg.m_exeName);

		PopulateInstallationsList();
	}
}

void CGameConfigPage::OnBnClickedEditInstallation()
{
	POSITION pos = m_installations.GetFirstSelectedItemPosition();
	if (!pos)
		return;

	int selItem = m_installations.GetNextSelectedItem(pos);
	CGameConfigPopup dlg(m_game, GetParent());

	dlg.m_gameName = m_game->GetName().c_str();
	dlg.m_version = m_installations.GetItemText(selItem, 0);
	dlg.m_installDir = m_installations.GetItemText(selItem, 1);
	dlg.m_exeName = m_installations.GetItemText(selItem, 2);
	dlg.m_additionalParams = m_installations.GetItemText(selItem, 3);

	if (dlg.DoModal() == IDOK)
	{
		SetModified(TRUE);

		m_installDirDict.Set(dlg.m_version, dlg.m_installDir);
		m_additionalParamDict.Set(dlg.m_version, dlg.m_additionalParams);
		m_exeNameDict.Set(dlg.m_version, dlg.m_exeName);

		PopulateInstallationsList();
	}
}

void CGameConfigPage::OnBnClickedDeleteInstallation()
{
	POSITION pos = m_installations.GetFirstSelectedItemPosition();
	if (!pos)
		return;

	int selItem = m_installations.GetNextSelectedItem(pos);

	CString version = m_installations.GetItemText(selItem, 0);
	if (version == "<default>")
	{
		AfxMessageBox("You cannot remove the default installation.");
		return;
	}
	m_installDirDict.Delete(version);
	m_additionalParamDict.Delete(version);
	m_exeNameDict.Delete(version);

	PopulateInstallationsList();
	SetModified(TRUE);
}

void CGameConfigPage::OnLvnItemchangedInstallations(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if ((pNMLV->uOldState & LVIS_SELECTED) != (pNMLV->uNewState & LVIS_SELECTED))
	{
		RefreshEnabledStates();
	}

	*pResult = 0;
}

void CGameConfigPage::RefreshEnabledStates()
{
	BOOL hasSelections = m_installations.GetSelectedCount() > 0;
	GetDlgItem(IDC_EDIT_INSTALLATION)->EnableWindow(m_installed && hasSelections);
	GetDlgItem(IDC_DELETE_INSTALLATION)->EnableWindow(m_installed && hasSelections);

	GetDlgItem(IDC_ADD_INSTALLATION)->EnableWindow(m_installed);
	GetDlgItem(IDC_INSTALLATIONS)->EnableWindow(m_installed);

}