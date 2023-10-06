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

struct IPINFO
{
	u_char Ttl;				// Time To Live
	u_char Tos;				// Type Of Service
	u_char IPFlags;			// IP flags
	u_char OptSize;			// Size of options data
	u_char FAR *Options;	// Options data buffer
};

struct ICMPECHO
{
	u_long Source;			// Source address
	u_long Status;			// IP status
	u_long RTTime;			// Round trip time in milliseconds
	u_short DataSize;		// Reply data size
	u_short Reserved;		// Unknown
	void FAR *pData;		// Reply data buffer
	IPINFO	ipInfo;			// Reply options
};

class ICMPPing
{
	ICMPPing();
	~ICMPPing();

public:
	static ICMPPing& GetInstance() { return m_instance; }
	int PingHost(const char *address);

	void SetEnabled(bool newVal) { m_enabled = newVal; }
protected:

	HMODULE m_module;
	bool m_initialised;
	bool m_enabled;

	static ICMPPing m_instance;

	HANDLE (WINAPI *m_pIcmpCreateFile)(VOID);
	BOOL (WINAPI * m_pIcmpCloseHandle)(HANDLE);
	DWORD (WINAPI * m_pIcmpSendEcho)(HANDLE,DWORD,LPVOID,WORD,IPINFO*,LPVOID,DWORD,DWORD);

};
