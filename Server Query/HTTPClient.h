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

#include <afxinet.h>

class HTTPClient : public CInternetSession
{
public:
	HTTPClient();
	virtual ~HTTPClient();

	DWORD Get(const CString& url, CString &resultString);
private:

	void ParsePath(const CString &url, CString &server, CString &path);
};


