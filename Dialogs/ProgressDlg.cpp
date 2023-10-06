// ProgressDlg.cpp : implementation file
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
#include "ProgressDlg.h"
#include ".\progressdlg.h"


// CProgressDlg dialog

IMPLEMENT_DYNAMIC(CProgressDlg, CDialog)
CProgressDlg::CProgressDlg(CWnd* pParent, int *progressValue, int maxValue, bool *cancelFlag, CString alternateLabel)
	: CDialog(CProgressDlg::IDD, pParent)
	, m_label(alternateLabel)
	, m_progressValue(progressValue)
	, m_maxValue(maxValue)
	, m_cancelFlag(cancelFlag)
{
}

CProgressDlg::~CProgressDlg()
{
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PROGRESS_LABEL, m_label);
	DDX_Control(pDX, IDC_PROGRESS, m_progressCtrl);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


BOOL CProgressDlg::OnInitDialog()
{
	BOOL lRetVal = CDialog::OnInitDialog();

	m_progressCtrl.SetRange(0, 1000);

	if (m_maxValue > 0)
	{
		OnTimer(0);

		// update the progress bar every 10th of a second.
		SetTimer(0, 100, 0);
	}

	return lRetVal;
}

void CProgressDlg::OnTimer(UINT_PTR nIDEvent)
{
	int progressValue = *m_progressValue;
	m_label.Format("Queried %d of %d servers", progressValue, m_maxValue);
	UpdateData(FALSE);
	
	m_progressCtrl.SetPos((progressValue * 1000) / m_maxValue);
}

void CProgressDlg::OnBnClickedCancel()
{
	*m_cancelFlag = true;	
	//OnCancel();
}
