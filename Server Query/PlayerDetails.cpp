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
#include ".\playerdetails.h"

PlayerDetails::PlayerDetails(void)
{
}

PlayerDetails::~PlayerDetails(void)
{
}


void PlayerDetails::StorePropertiesXML(idStr &output)
{
	output += "\t\t\t<player ";

	int numProperties = m_properties.GetNumKeyVals();
	for (int j=0; j<numProperties; j++)
	{
		const idKeyValue *thisPair = m_properties.GetKeyVal(j);
		output += va("%s=\"%s\" ", thisPair->GetKey().c_str(), XMLEscapedString(thisPair->GetValue()).c_str());
	}

	output += "/>\n";
}

PlayerDetails *PlayerDetails::LoadFromXML(MSXML2::IXMLDOMElementPtr pPlayerDetailsNode)
{
	PlayerDetails *newPlayer = new PlayerDetails;

	bool lRet = false;

	CComPtr<MSXML2::IXMLDOMNamedNodeMap> lAttributeList;
	pPlayerDetailsNode->get_attributes(&lAttributeList);

	if(lAttributeList)
	{
		CComPtr<MSXML2::IXMLDOMNode> lAttribute = lAttributeList->nextNode();
		CString lText;
		CComBSTR lBSTR;

		while(lAttribute)
		{
			lBSTR.Empty();
			
			HRESULT lResult = lAttribute->get_nodeName(&lBSTR);

			if(lResult == S_OK)
			{
				lText = lBSTR;
				idStr value = lAttribute->GetnodeValue();

				newPlayer->m_properties.Set(lText, value.c_str());
			}

			lAttribute = lAttributeList->nextNode();
		}
	}

	return newPlayer;
}
