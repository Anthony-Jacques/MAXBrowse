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
#include "HTTPClient.h"

HTTPClient::HTTPClient()
		: CInternetSession(NULL, 1, PRE_CONFIG_INTERNET_ACCESS, NULL, NULL, 0)
{
}

HTTPClient::~HTTPClient()
{
	this->Close();
}

DWORD HTTPClient::Get(const CString& url, CString& resultString)
{
	int errorCode = 0;
	resultString = "";

	CString path, server;
	ParsePath(url, server, path);

	CHttpConnection* pHttpConnection = NULL;
	try
	{
		pHttpConnection = GetHttpConnection(server);

		if (!pHttpConnection)
		{
			errorCode = -1;
		}
	}
	catch (CInternetException* pException)
	{
		char buffer[1023];
		pException->GetErrorMessage(buffer, 1023);
		resultString = buffer;
		errorCode = pException->m_dwError;
		pException->Delete();
	}

	CHttpFile* pHttpFile = NULL;
	if (errorCode == 0)
	{
		try
		{
			pHttpFile = pHttpConnection->OpenRequest(NULL, path);
			if (!pHttpFile)
			{
				errorCode = -2;
			}
		}
		catch (CInternetException* pException)
		{
			char buffer[1023];
			pException->GetErrorMessage(buffer, 1023);
			resultString = buffer;
			errorCode = pException->m_dwError;
			pException->Delete();
		}
	}

	if (errorCode == 0)
	{
		try
		{
			if (!pHttpFile->SendRequest())
			{
				errorCode = -3;
			}
		}
		catch (CInternetException* pException)
		{
			char buffer[1023];
			pException->GetErrorMessage(buffer, 1023);
			resultString = buffer;
			errorCode = pException->m_dwError;
			pException->Delete();
		}
	}

	if (errorCode == 0)
	{
		DWORD retCode;
		if (!pHttpFile->QueryInfoStatusCode(retCode))
		{
			return -4;
		}
		else if (retCode != HTTP_STATUS_OK)
		{
			errorCode = retCode;
		}
	}

	if (errorCode == 0)
	{
		char buf[2];
		while (pHttpFile->Read(buf, 1) > 0)
		{
			resultString += buf[0];
		}
	}

	if (pHttpFile)
	{
		pHttpFile->Close();
		delete pHttpFile;
	}

	if (pHttpConnection)
	{
		pHttpConnection->Close();
		delete pHttpConnection;
	}

	return errorCode;
}

void HTTPClient::ParsePath(const CString &url, CString &server, CString &path)
{
	int serverStart = 0;
	if (url.Left(strlen("http://")).Compare("http://") == 0)
	{
		serverStart = strlen("http://");
	}
	if (url.Left(strlen("https://")).Compare("https://") == 0)
	{
		serverStart = strlen("https://");
	}

	int serverEnd = url.Find('/', serverStart);
	int pathLength = url.GetLength() - serverEnd;

	if ((pathLength > 0) && serverEnd > 0)
	{
		server = url.Mid(serverStart, serverEnd - serverStart);
		path = url.Mid(serverEnd, pathLength);
	}
	else
	{
		path = "";
		server = "";
	}
}