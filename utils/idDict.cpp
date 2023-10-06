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
#include "iddict.h"

// Note - this code does not relate to the idLib code in Q4, but for a bit I was considering
// trying to embed the browser capabilities inside Q4 itself, and therefore it seemed a good idea
// to match the idLib names.

void idDict::Copy( const idDict &other )
{
	POSITION pos = other.m_map.GetStartPosition();

	CString key, value;
	while (pos)
	{
		other.m_map.GetNextAssoc(pos, key, value);
		m_map.SetAt(key, value);
	}

	m_pos = NULL; 
	m_posIndex = -1;
}

void idDict::Set( const char *key, const char *value )
{
	CString lowerKey = key;
	m_map.SetAt(lowerKey.MakeLower(), value); 
	m_pos = NULL; 
	m_posIndex = -1; 
}

void idDict::SetFloat( const char *key, float val )
{
	CString value;
	value.Format("%f", val);
	Set(key, value);
}

void idDict::SetInt( const char *key, int val )
{
	CString value;
	value.Format("%d", val);
	Set(key, value);
}

void idDict::SetBool( const char *key, bool val )
{
	CString value;
	value.Format("%d", (int)val);
	Set(key, value);
}


const char *idDict::GetString( const char *key, const char *defaultString ) const
{
	CString keyLowerCase = key;
	keyLowerCase = keyLowerCase.MakeLower();
	CString value;
	if (!m_map.Lookup(keyLowerCase, value))
		return defaultString;

	return value;
}

float idDict::GetFloat( const char *key, const char *defaultString ) const
{
	CString value;
	if (!m_map.Lookup(key, value))
		value = defaultString;

	return (float)atof(value);
}

int idDict::GetInt( const char *key, const char *defaultString ) const
{
	CString value;
	if (!m_map.Lookup(key, value))
		value = defaultString;

	return atoi(value);
}
bool idDict::GetBool( const char *key, const char *defaultString ) const
{
	CString value;
	if (!m_map.Lookup(key, value))
		value = defaultString;

	return (!!atoi(value));
}

const idKeyValue *idDict::GetKeyVal( int index ) const
{
	if (!m_pos || index < (m_posIndex - 1))
	{
		m_pos = m_map.GetStartPosition();
		m_posIndex = 0;
	}
	else if (index == (m_posIndex - 1))
		return &m_lastKeyValue;

	CString key, value;
	do
	{
		m_map.GetNextAssoc(m_pos, key, value);
		m_posIndex++;
	} while (m_posIndex < index && m_pos);

	if (m_posIndex != (index +1))
		return NULL;

	m_lastKeyValue.SetKey(key);
	m_lastKeyValue.SetValue(value);

	return &m_lastKeyValue;
}