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

#include "StdAfx.h"
#include "HTTPMaster.h"
#include "ETQWServer.h"
#include "HTTPClient.h"

HTTPMaster::HTTPMaster(const idStr &host, const unsigned short port)
	: MasterServer(host, port)
{
}


HTTPMaster::~HTTPMaster(void)
{
}

int HTTPMaster::ImportIPsFromURL(CString url)
{
	CString webPage;
	HTTPClient httpClient;
	int returnCode = httpClient.Get(url, webPage);

	if (returnCode == 0)
	{
		return ImportIPs(webPage);
	}
	else
	{
		CString message;
		message.Format("Error returned from %s - %d\n\t%s", (LPCTSTR)url, returnCode, (LPCTSTR)webPage);
		AfxMessageBox(message);
		return 0;
	}
}

bool HTTPMaster::Query(int filter)
{
	return ImportIPsFromURL(m_host.c_str());
}
