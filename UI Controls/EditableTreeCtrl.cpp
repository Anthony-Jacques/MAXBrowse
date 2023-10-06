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
#include "EditableTreeCtrl.h"

BOOL CEditableTreeCtrl::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )
	{
	      // When an item is being edited make sure the edit control
	      // receives certain important key strokes
		if( GetEditControl() && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_DELETE
			|| pMsg->wParam == VK_ESCAPE || GetKeyState(VK_CONTROL)))
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;    // DO NOT process further
		}
	}

	return CTreeCtrl::PreTranslateMessage(pMsg);
}