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

#include <vector>

template< class type > class idList : public std::vector<type> 
{
public:

	idList() {}
	~idList<type>() { }

	void Clear( void ) { clear(); }
	int Num( void ) const { return (int)size(); }

	int	Append( const type & obj ) { push_back(obj); return Num(); }
	int	Append( const idList<type> &other ) { push_back(other); return Num(); }

	bool RemoveIndex(int index)
	{
		std::vector<type>::iterator iter;
		int thisVal = 0;
		for (iter = begin() ; iter != end() && (thisVal < index); iter++)
			thisVal++;

		if (iter != end())
		{
			erase(iter);
			return true;
		}

		return false;
	}

	void DeleteContents( bool clear ) 
	{ 
		while (!empty()) 
		{ 
			delete back(); 
			pop_back(); 
		} 
	}
};

