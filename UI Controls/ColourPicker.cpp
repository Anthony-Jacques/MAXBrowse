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
#include "ColourPicker.h"

// CColourPicker
IMPLEMENT_DYNAMIC(CColourPicker, CButton)
CColourPicker::CColourPicker()
{
	m_colour = RGB(255,255,255);
	m_customBrush.CreateSolidBrush(m_colour);
}

CColourPicker::~CColourPicker()
{
}

BEGIN_MESSAGE_MAP(CColourPicker, CButton)
	ON_CONTROL_REFLECT(BN_CLICKED, OnBnClicked)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CColourPicker message handlers

void CColourPicker::OnBnClicked()
{
	CColorDialog dlg(m_colour, 0, GetParent());
	if (dlg.DoModal() == IDOK)
		SetColour(dlg.m_cc.rgbResult);
}

void CColourPicker::SetColour(COLORREF colour)
{
	m_colour = colour;
	m_customBrush.DeleteObject();
	m_customBrush.CreateSolidBrush(m_colour);

	if (m_hWnd)
		Invalidate();
}

BOOL CColourPicker::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(rect);

	pDC->Rectangle(rect);
	pDC->Draw3dRect(rect, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHIGHLIGHT));

	rect.DeflateRect(1,1);
	pDC->Draw3dRect(rect, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHIGHLIGHT));

	rect.DeflateRect(1,1);
	pDC->FillRect(rect, &m_customBrush);

	return FALSE;
}

void CColourPicker::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}
