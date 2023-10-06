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
#include "IPNationalityMap.h"

IPRange::IPRange(unsigned int s, unsigned int e, const char *c)
	: start(s), end(e)
{
	strcpy(countryCode, c);
}

IPRange::~IPRange(void)
{
}


IPNationalityMap *IPNationalityMap::theInstance = NULL;

IPNationalityMap::IPNationalityMap(void)
{
	m_ipNationalityData = NULL;
	m_countNationalityData = 0;

	InitialiseFlagMap();

	theInstance = this;
}

IPNationalityMap::~IPNationalityMap(void)
{
	theInstance = NULL;

	if (m_ipNationalityData)
		delete [] m_ipNationalityData;
}


void IPNationalityMap::LoadIPDatabase()
{
	if (m_ipNationalityData)
		delete [] m_ipNationalityData;

	m_countNationalityData = 0;

	CFile ipData;
	if (ipData.Open("IPData.dat", CFile::OpenFlags::modeRead))
	{
		ipData.Read(&m_countNationalityData, 4);
		
		m_ipNationalityData = new IPRange[m_countNationalityData];
		ipData.Read(m_ipNationalityData, m_countNationalityData * sizeof(IPRange));

		ipData.Close();
	}
}


const char *IPNationalityMap::GetNationality(const char *dottedQuad) const
{
	int quadPart[4] = { 0,0,0,0};
	sscanf(dottedQuad, "%d.%d.%d.%d", &quadPart[0], &quadPart[1], &quadPart[2], &quadPart[3]);

	unsigned int numericVal = quadPart[0] << 24 | quadPart[1] << 16 | quadPart[2] << 8 | quadPart[3];

	for (int i=0; i<m_countNationalityData; i++)
	{
		if (m_ipNationalityData[i].start <= numericVal && numericVal <= m_ipNationalityData[i].end)
			return m_ipNationalityData[i].countryCode;
	}

	return "";
}

int IPNationalityMap::GetFlagID(const char *nationality) const
{
	for (int i=IDR_FLAG1; i <= IDR_FLAG212; i++)
	{
		if (stricmp(nationality, m_flagMap[i]) == 0)
			return i;
	}
 
	return -1;
}

int IPNationalityMap::GetFlagImageID(const char* dottedQuad) const
{
	int id = GetFlagID(dottedQuad);
	
	if (id < 0)
		id = -1;
	else
		id -= IDR_FLAG1;

	return id;
}


void IPNationalityMap::InitialiseFlagMap()
{
	m_flagMap[IDR_FLAG1] = "ad";
	m_flagMap[IDR_FLAG2] = "ae";
	m_flagMap[IDR_FLAG3] = "ag";
	m_flagMap[IDR_FLAG4] = "ai";
	m_flagMap[IDR_FLAG5] = "al";
	m_flagMap[IDR_FLAG6] = "am";
	m_flagMap[IDR_FLAG7] = "an";
	m_flagMap[IDR_FLAG8] = "ao";
	m_flagMap[IDR_FLAG9] = "ar";
	m_flagMap[IDR_FLAG10] = "as";
	m_flagMap[IDR_FLAG11] = "at";
	m_flagMap[IDR_FLAG12] = "au";
	m_flagMap[IDR_FLAG13] = "aw";
	m_flagMap[IDR_FLAG14] = "az";
	m_flagMap[IDR_FLAG15] = "ba";
	m_flagMap[IDR_FLAG16] = "bb";
	m_flagMap[IDR_FLAG17] = "bd";
	m_flagMap[IDR_FLAG18] = "be";
	m_flagMap[IDR_FLAG19] = "bf";
	m_flagMap[IDR_FLAG20] = "bg";
	m_flagMap[IDR_FLAG21] = "bh";
	m_flagMap[IDR_FLAG22] = "bi";
	m_flagMap[IDR_FLAG23] = "bj";
	m_flagMap[IDR_FLAG24] = "bm";
	m_flagMap[IDR_FLAG25] = "bn";
	m_flagMap[IDR_FLAG26] = "bo";
	m_flagMap[IDR_FLAG27] = "br";
	m_flagMap[IDR_FLAG28] = "bs";
	m_flagMap[IDR_FLAG29] = "bt";
	m_flagMap[IDR_FLAG30] = "bw";
	m_flagMap[IDR_FLAG31] = "by";
	m_flagMap[IDR_FLAG32] = "bz";
	m_flagMap[IDR_FLAG33] = "ca";
	m_flagMap[IDR_FLAG34] = "cd";
	m_flagMap[IDR_FLAG35] = "cf";
	m_flagMap[IDR_FLAG36] = "cg";
	m_flagMap[IDR_FLAG37] = "ch";
	m_flagMap[IDR_FLAG38] = "ci";
	m_flagMap[IDR_FLAG39] = "cl";
	m_flagMap[IDR_FLAG40] = "cm";
	m_flagMap[IDR_FLAG41] = "cn";
	m_flagMap[IDR_FLAG42] = "co";
	m_flagMap[IDR_FLAG43] = "cr";
	m_flagMap[IDR_FLAG44] = "cu";
	m_flagMap[IDR_FLAG45] = "cv";
	m_flagMap[IDR_FLAG46] = "cy";
	m_flagMap[IDR_FLAG47] = "cz";
	m_flagMap[IDR_FLAG48] = "de";
	m_flagMap[IDR_FLAG49] = "dj";
	m_flagMap[IDR_FLAG50] = "dk";
	m_flagMap[IDR_FLAG51] = "dm";
	m_flagMap[IDR_FLAG52] = "do";
	m_flagMap[IDR_FLAG53] = "dz";
	m_flagMap[IDR_FLAG54] = "ec";
	m_flagMap[IDR_FLAG55] = "ee";
	m_flagMap[IDR_FLAG56] = "eg";
	m_flagMap[IDR_FLAG57] = "er";
	m_flagMap[IDR_FLAG58] = "es";
	m_flagMap[IDR_FLAG59] = "et";
	m_flagMap[IDR_FLAG60] = "eu";
	m_flagMap[IDR_FLAG61] = "fi";
	m_flagMap[IDR_FLAG62] = "fj";
	m_flagMap[IDR_FLAG63] = "fm";
	m_flagMap[IDR_FLAG64] = "fo";
	m_flagMap[IDR_FLAG65] = "fr";
	m_flagMap[IDR_FLAG66] = "ga";
	m_flagMap[IDR_FLAG67] = "gb";
	m_flagMap[IDR_FLAG68] = "gd";
	m_flagMap[IDR_FLAG69] = "gg";
	m_flagMap[IDR_FLAG70] = "gh";
	m_flagMap[IDR_FLAG71] = "gi";
	m_flagMap[IDR_FLAG72] = "gl";
	m_flagMap[IDR_FLAG73] = "gm";
	m_flagMap[IDR_FLAG74] = "gn";
	m_flagMap[IDR_FLAG75] = "gq";
	m_flagMap[IDR_FLAG76] = "gr";
	m_flagMap[IDR_FLAG77] = "gt";
	m_flagMap[IDR_FLAG78] = "gu";
	m_flagMap[IDR_FLAG79] = "gw";
	m_flagMap[IDR_FLAG80] = "gy";
	m_flagMap[IDR_FLAG81] = "hk";
	m_flagMap[IDR_FLAG82] = "hn";
	m_flagMap[IDR_FLAG83] = "hr";
	m_flagMap[IDR_FLAG84] = "ht";
	m_flagMap[IDR_FLAG85] = "hu";
	m_flagMap[IDR_FLAG86] = "id";
	m_flagMap[IDR_FLAG87] = "ie";
	m_flagMap[IDR_FLAG88] = "il";
	m_flagMap[IDR_FLAG89] = "in";
	m_flagMap[IDR_FLAG90] = "iq";
	m_flagMap[IDR_FLAG91] = "ir";
	m_flagMap[IDR_FLAG92] = "is";
	m_flagMap[IDR_FLAG93] = "it";
	m_flagMap[IDR_FLAG94] = "je";
	m_flagMap[IDR_FLAG95] = "jm";
	m_flagMap[IDR_FLAG96] = "jo";
	m_flagMap[IDR_FLAG97] = "jp";
	m_flagMap[IDR_FLAG98] = "ke";
	m_flagMap[IDR_FLAG99] = "kg";
	m_flagMap[IDR_FLAG100] = "kh";
	m_flagMap[IDR_FLAG101] = "ki";
	m_flagMap[IDR_FLAG102] = "kn";
	m_flagMap[IDR_FLAG103] = "kp";
	m_flagMap[IDR_FLAG104] = "kr";
	m_flagMap[IDR_FLAG105] = "kw";
	m_flagMap[IDR_FLAG106] = "ky";
	m_flagMap[IDR_FLAG107] = "kz";
	m_flagMap[IDR_FLAG108] = "la";
	m_flagMap[IDR_FLAG109] = "lb";
	m_flagMap[IDR_FLAG110] = "lc";
	m_flagMap[IDR_FLAG111] = "li";
	m_flagMap[IDR_FLAG112] = "lk";
	m_flagMap[IDR_FLAG113] = "lr";
	m_flagMap[IDR_FLAG114] = "ls";
	m_flagMap[IDR_FLAG115] = "lt";
	m_flagMap[IDR_FLAG116] = "lu";
	m_flagMap[IDR_FLAG117] = "lv";
	m_flagMap[IDR_FLAG118] = "ly";
	m_flagMap[IDR_FLAG119] = "ma";
	m_flagMap[IDR_FLAG120] = "mc";
	m_flagMap[IDR_FLAG121] = "md";
	m_flagMap[IDR_FLAG122] = "mg";
	m_flagMap[IDR_FLAG123] = "mh";
	m_flagMap[IDR_FLAG124] = "mk";
	m_flagMap[IDR_FLAG125] = "ml";
	m_flagMap[IDR_FLAG126] = "mm";
	m_flagMap[IDR_FLAG127] = "mn";
	m_flagMap[IDR_FLAG128] = "mp";
	m_flagMap[IDR_FLAG129] = "mq";
	m_flagMap[IDR_FLAG130] = "mr";
	m_flagMap[IDR_FLAG131] = "ms";
	m_flagMap[IDR_FLAG132] = "mt";
	m_flagMap[IDR_FLAG133] = "mu";
	m_flagMap[IDR_FLAG134] = "mv";
	m_flagMap[IDR_FLAG135] = "mw";
	m_flagMap[IDR_FLAG136] = "mx";
	m_flagMap[IDR_FLAG137] = "my";
	m_flagMap[IDR_FLAG138] = "mz";
	m_flagMap[IDR_FLAG139] = "na";
	m_flagMap[IDR_FLAG140] = "ne";
	m_flagMap[IDR_FLAG141] = "ng";
	m_flagMap[IDR_FLAG142] = "ni";
	m_flagMap[IDR_FLAG143] = "nl";
	m_flagMap[IDR_FLAG144] = "no";
	m_flagMap[IDR_FLAG145] = "np";
	m_flagMap[IDR_FLAG146] = "nr";
	m_flagMap[IDR_FLAG147] = "nu";
	m_flagMap[IDR_FLAG148] = "nz";
	m_flagMap[IDR_FLAG149] = "om";
	m_flagMap[IDR_FLAG150] = "pa";
	m_flagMap[IDR_FLAG151] = "pe";
	m_flagMap[IDR_FLAG152] = "pg";
	m_flagMap[IDR_FLAG153] = "ph";
	m_flagMap[IDR_FLAG154] = "pk";
	m_flagMap[IDR_FLAG155] = "pl";
	m_flagMap[IDR_FLAG156] = "pn";
	m_flagMap[IDR_FLAG157] = "pr";
	m_flagMap[IDR_FLAG158] = "pt";
	m_flagMap[IDR_FLAG159] = "pw";
	m_flagMap[IDR_FLAG160] = "py";
	m_flagMap[IDR_FLAG161] = "qa";
	m_flagMap[IDR_FLAG162] = "ro";
	m_flagMap[IDR_FLAG163] = "ru";
	m_flagMap[IDR_FLAG164] = "rw";
	m_flagMap[IDR_FLAG165] = "sa";
	m_flagMap[IDR_FLAG166] = "sb";
	m_flagMap[IDR_FLAG167] = "sc";
	m_flagMap[IDR_FLAG168] = "sd";
	m_flagMap[IDR_FLAG169] = "se";
	m_flagMap[IDR_FLAG170] = "sg";
	m_flagMap[IDR_FLAG171] = "si";
	m_flagMap[IDR_FLAG172] = "sk";
	m_flagMap[IDR_FLAG173] = "sl";
	m_flagMap[IDR_FLAG174] = "sm";
	m_flagMap[IDR_FLAG175] = "sn";
	m_flagMap[IDR_FLAG176] = "so";
	m_flagMap[IDR_FLAG177] = "sov";
	m_flagMap[IDR_FLAG178] = "sr";
	m_flagMap[IDR_FLAG179] = "st";
	m_flagMap[IDR_FLAG180] = "sv";
	m_flagMap[IDR_FLAG181] = "sy";
	m_flagMap[IDR_FLAG182] = "sz";
	m_flagMap[IDR_FLAG183] = "td";
	m_flagMap[IDR_FLAG184] = "tg";
	m_flagMap[IDR_FLAG185] = "th";
	m_flagMap[IDR_FLAG186] = "tj";
	m_flagMap[IDR_FLAG187] = "tm";
	m_flagMap[IDR_FLAG188] = "tn";
	m_flagMap[IDR_FLAG189] = "to";
	m_flagMap[IDR_FLAG190] = "tr";
	m_flagMap[IDR_FLAG191] = "tt";
	m_flagMap[IDR_FLAG192] = "tv";
	m_flagMap[IDR_FLAG193] = "tw";
	m_flagMap[IDR_FLAG194] = "tz";
	m_flagMap[IDR_FLAG195] = "ua";
	m_flagMap[IDR_FLAG196] = "ug";
	m_flagMap[IDR_FLAG197] = "us";
	m_flagMap[IDR_FLAG198] = "uy";
	m_flagMap[IDR_FLAG199] = "uz";
	m_flagMap[IDR_FLAG200] = "va";
	m_flagMap[IDR_FLAG201] = "vc";
	m_flagMap[IDR_FLAG202] = "ve";
	m_flagMap[IDR_FLAG203] = "vg";
	m_flagMap[IDR_FLAG204] = "vn";
	m_flagMap[IDR_FLAG205] = "vu";
	m_flagMap[IDR_FLAG206] = "ws";
	m_flagMap[IDR_FLAG207] = "ye";
	m_flagMap[IDR_FLAG208] = "yu";
	m_flagMap[IDR_FLAG209] = "za";
	m_flagMap[IDR_FLAG210] = "zm";
	m_flagMap[IDR_FLAG211] = "zw";
	m_flagMap[IDR_FLAG212] = "rs";
}

void IPNationalityMap::CreateImageList()
{
	m_imageList.Create(18, 12, ILC_MASK | ILC_COLOR32, IDR_FLAG212 + 1, IDR_FLAG212 + 1);
//	m_imageList.Create(30, 15, ILC_MASK | ILC_COLOR32, IDR_FLAG211 + 1, IDR_FLAG211 + 1);
//	m_imageList.Create(30, 15, ILC_MASK | ILC_COLOR32, IDR_FLAG211 + 1, IDR_FLAG211 + 1);
//	m_imageList.Create(60, 30, ILC_MASK | ILC_COLOR32, IDR_FLAG211 + 1, IDR_FLAG211 + 1);
//	m_imageList.SetImageCount(IDR_FLAG211 - IDR_FLAG1 + 1);
	
	for (int i=IDR_FLAG1; i<=IDR_FLAG212; i++)
	{
		CBitmap bmp;
		BOOL value = bmp.LoadBitmap(i);

		int index = m_imageList.Add(&bmp, RGB(31,41,59));
		int count = m_imageList.GetImageCount();

		int expectedIndex = i - IDR_FLAG1;
		if (index != expectedIndex)
		{
			int j=0;
		}
	}
}