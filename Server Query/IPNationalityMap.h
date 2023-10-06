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

#include "resource.h"

class IPRange
{
public:
	IPRange(unsigned int s, unsigned int e, const char *c);
	IPRange() { start = 0; end = -1; countryCode[0]='\0'; }
	~IPRange(void);

	unsigned int start;
	unsigned int end;
	char countryCode[4];
};


class IPNationalityMap
{
public:
	IPNationalityMap(void);
	~IPNationalityMap(void);

public:
	void LoadIPDatabase();
	void CreateImageList();

	int GetFlagImageID(const char* dottedQuad) const;
	const char *GetNationality(const char *dottedQuad) const;

	const CImageList *GetImageList() const { return &m_imageList; }
	static IPNationalityMap *GetIPNationalityMap() { return theInstance; }

protected:
	int GetFlagID(const char* dottedQuad) const;

	IPRange *m_ipNationalityData;
	int m_countNationalityData;

	void InitialiseFlagMap();

	char *m_flagMap[IDR_FLAG212];
	CImageList m_imageList;

	static IPNationalityMap *theInstance;
};
