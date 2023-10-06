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

idStr XMLEscapedString(const idStr oldStr);

CComPtr<MSXML2::IXMLDOMNode> XMLGetAttribute(MSXML2::IXMLDOMNode *aNode, char *aAttributeName);

bool GetRegistryString(CString& result, HKEY regParent, CString key, CString regValue, CString defaultStr);


class CWindowRect :public CRect
{
public:
	CWindowRect(HWND hWnd)
	{
		::GetWindowRect(hWnd, this);
	};
	CWindowRect(const CWnd* pWnd)
	{
		::GetWindowRect(pWnd->GetSafeHwnd(), this);
	};
};
