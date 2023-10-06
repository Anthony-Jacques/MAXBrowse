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
#include <fstream>


using namespace std;


char *va( const char *fmt, ... ) 
{
	va_list argptr;
	static int index = 0;
	static char string[VA_NUM_BUFS][VA_BUF_LEN];

	char *buf = string[index];

	index = (index + 1) & (VA_NUM_BUFS - 1);

	va_start( argptr, fmt );
	vsprintf( buf, fmt, argptr );
	va_end( argptr );

	return buf;
}


idStr XMLEscapedString(const idStr oldStr)
{
	idStr escapedStr = oldStr;
	std::string test;
	escapedStr.Replace("&", "&amp;"); 
	escapedStr.Replace("<", "&lt;"); 
	escapedStr.Replace(">", "&gt;"); 
	escapedStr.Replace("\"", "&quot;");
	escapedStr.Replace("'", "&apos;");

	idStr highCharEscaped = "";
	int len = escapedStr.Length();
	for (int i=0; i<len; i++)
	{
		unsigned char thisChar = (unsigned char)escapedStr[i];
		if (thisChar > 127)
			highCharEscaped += va("&#%d;", (int)thisChar);
		else if (thisChar > 31)
			highCharEscaped += escapedStr[i];
	}

	return highCharEscaped;
}


CComPtr<MSXML2::IXMLDOMNode> XMLGetAttribute(MSXML2::IXMLDOMNode *aNode, char *aAttributeName)
{
	bool lRet = false;

	CComPtr<MSXML2::IXMLDOMNamedNodeMap> lAttributeList;
	aNode->get_attributes(&lAttributeList);

	if(lAttributeList)
	{
		CComPtr<MSXML2::IXMLDOMNode> lAttribute = lAttributeList->nextNode();

		while(lAttribute)
		{
			CComBSTR lText;
			lText.Empty();
			
			HRESULT lResult = lAttribute->get_nodeName(&lText);

			if(lResult == S_OK && lText == aAttributeName)
			{
				//lAttribute.CopyTo(aAttribute);
				return lAttribute;
			}

			lAttribute = lAttributeList->nextNode();
		}
	}

	return NULL;
}


bool GetRegistryString(CString& result, HKEY regParent, CString key, CString regValue, CString defaultStr)
{
	result = defaultStr;

	HKEY hKey;
	DWORD retVal = RegOpenKeyEx(regParent, key, 0, KEY_QUERY_VALUE, &hKey);
	
	if (retVal == ERROR_SUCCESS)
	{
		BYTE resBuff[512];
		DWORD strLen = 512;
		retVal = RegQueryValueEx(hKey, regValue, NULL, NULL, resBuff, &strLen);
		if (retVal == ERROR_SUCCESS)
			result = resBuff;

		RegCloseKey(hKey);
	}

	return (retVal == ERROR_SUCCESS);
}
