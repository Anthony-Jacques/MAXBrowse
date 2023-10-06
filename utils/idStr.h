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

#include <string>
#include <algorithm>

#define	ESCAPE_CHAR	'^'

class idStr : public std::string
{
public:

	idStr() : std::string() {}
	idStr(const char *str) : std::string(str) {}
	idStr(const char *str, int start, int len) : std::string(str, start, len) {}
	virtual ~idStr() {} 

	static int ICmp(const char *first, const char *second) { return _stricmp(first, second); }

	int ICmp(const char *other) const { return _stricmp(c_str(), other); }
	int ICmpn(const char *other, int n) { return _strnicmp(c_str(), other, n); }

	int	Find( const char *text, bool casesensitive = true, int start = 0, int end = -1 ) const
	{
		ASSERT(start == 0 && end == -1);

		if (casesensitive)
			return (find(text) != std::string::npos) ? 1 : -1 ;
		else
		{
			std::string thisText = c_str();
			std::string otherText = text;

			transform(thisText.begin(), thisText.end(), thisText.begin(), (int(*)(int)) tolower);
			transform(otherText.begin(), otherText.end(), otherText.begin(), (int(*)(int)) tolower);

			return (thisText.find(otherText) != std::string::npos) ? 1 : -1 ;
		}
	}

		
	void Clear() { clear(); }

	int Length() const { return (int)length(); }
	static int Length(const char *str) { return strlen(str); }

	idStr(_variant_t xmlVal) 
	{ 
		CString temp; 
		int n = SysStringLen(xmlVal.bstrVal);
		char *buf = temp.GetBuffer(n + 3);

		for (int i = 0; i < n; i++)
			buf[i] = (char)xmlVal.bstrVal[i];
		buf[n] = 0;
		temp.ReleaseBuffer();

		*this = (const char *)temp; 
	}
	idStr(BSTR xmlVal) 
	{ 
		CString temp; 

		int n = SysStringLen(xmlVal);
		char *buf = temp.GetBuffer(n + 3);

		for (int i = 0; i < n; i++)
			buf[i] = (char)xmlVal[i];
		buf[n] = 0;
		temp.ReleaseBuffer();

		*this = (const char*)temp; 
	}

	int	Replace( const char *old, const char *nw )
	{
		int count = 0;
		int oldLen = strlen(old);
		int newLen = strlen(nw); 

		size_t foundAt = find( old );
		while (foundAt != npos)
		{
			replace(foundAt, oldLen, nw);
			foundAt = find(old, foundAt + newLen);
			count++;
		}

		return count;
	}


	idStr &RemoveEscapes(bool q3Escapes)
	{
		int s = 0;
		int d = 0;

		
		idStr src = *this;
		int srcLen = this->Length();

		*this = "";

		for (int s=0; s<srcLen; s++)
		{
			int esc = q3Escapes ? IsQ3Escape(src.c_str() + s, 0) : IsEscape(src.c_str() + s, 0);
			if (esc) 
			{
				s += (esc - 1); // -1 as we already moved on cos of the s++
				continue;
			}		
			else 
			{
				*this += src[s];
				if (src[s] == ESCAPE_CHAR && src[s + 1])
					s++;
			}
		}

		return *this;
	}


	int IsEscape(const char *s, int* type)  
	{
		if (!s || *s != ESCAPE_CHAR || *(s+1) == ESCAPE_CHAR) 
			return 0;

		switch ( *(s+1) ) 
		{
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		case ':':
			return 2;

		case '-': case '+':
			return 2;
			
		case 'r': case 'R': 
			return 2;
			
		case 'c': case 'C':
			if ( *(s+2) &&  *(s+3) &&  *(s+4) )
				return 5;

			return 0;
			
		case 'n': case 'N':
			if ( *(s+2) ) 
				return 3;

			return 0;
			
		case 'i': case 'I':
			if ( *(s+2) && *(s+3) && *(s+4) )
				return 5;
			return 0;		
		}
		
		return 0;
	}

	int IsQ3Escape(const char *s, int* type)  
	{
		if (!s || *s != ESCAPE_CHAR || *(s+1) == ESCAPE_CHAR) 
			return 0;

		switch ( *(s+1) ) 
		{
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			return 2;
		}
		
		return 0;
	}

	const idStr &Right(int startFrom)
	{
		static idStr tempStr;
		tempStr = this->substr(Length() - startFrom).c_str();

		return tempStr;
	}
};




#define VA_NUM_BUFS	4			// must be a power of 2
#define	VA_BUF_LEN	10000

extern char *va( const char *fmt, ... );