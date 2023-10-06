#pragma once

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


class idKeyValue
{
public:
	idKeyValue() {};

	const idStr &GetKey() const { return key; }
	const idStr &GetValue() const { return value; }

	bool operator==( const idKeyValue &other) const { return (key == other.key && value == other.value); }

	// should only be used by idDict
	idKeyValue(const idStr &aKey, const idStr &aValue) : key(aKey), value(aValue) {}
	void SetKey(CString aKey) { key = (const char*)aKey; }
	void SetValue(CString aValue) { value = (const char*)aValue; }

private:
	idStr	key;
	idStr	value;
};



class idDict {
public:
	idDict( void ) { m_pos = NULL; m_posIndex = -1; }
	idDict( const idDict &other ) { Clear(); Copy(other); } 
	~idDict( void ) {}

	void				Copy( const idDict &other );

	idDict &			operator=( const idDict &other ) { Clear(); Copy(other);  return *this; }
	void				TransferKeyValues( idDict &other ) { Clear(); Copy(other); other.Clear(); }

	void				Clear() { m_map.RemoveAll(); m_pos = NULL; m_posIndex = -1; }

	void				Set( const char *key, const char *value );
	void				SetFloat( const char *key, float val );
	void				SetInt( const char *key, int val );
	void				SetBool( const char *key, bool val );
	
	const char *		GetString( const char *key, const char *defaultString = "" ) const;
	float				GetFloat( const char *key, const char *defaultString = "0" ) const;
	int					GetInt( const char *key, const char *defaultString = "0" ) const;
	bool				GetBool( const char *key, const char *defaultString = "0" ) const;

	int					GetNumKeyVals( void ) const { return (int)m_map.GetCount(); }

	const idKeyValue *	GetKeyVal( int index ) const;

	void				Delete( const char *key ) { m_map.RemoveKey(key); m_pos = NULL; m_posIndex = -1; }

	// finds the next key/value pair with the given key prefix.
	// lastMatch can be used to do additional searches past the first match.
//	const idKeyValue *	MatchPrefix( const char *prefix, const idKeyValue *lastMatch = NULL ) const;

protected:
	CMapStringToString	m_map;

	mutable POSITION	m_pos;
	mutable int			m_posIndex;
	mutable idKeyValue	m_lastKeyValue;
};
