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
#include "ICMPPing.h"

ICMPPing ICMPPing::m_instance;

ICMPPing::ICMPPing()
{
	m_initialised = false;
	m_enabled = true;

	m_module = LoadLibrary("ICMP.DLL");
	if (m_module == NULL)
		return;

	m_pIcmpCreateFile = (HANDLE (WINAPI *)(void))GetProcAddress(m_module,"IcmpCreateFile");
	m_pIcmpCloseHandle = (BOOL (WINAPI *)(HANDLE))GetProcAddress(m_module,"IcmpCloseHandle");
	m_pIcmpSendEcho = (DWORD (WINAPI *)(HANDLE,DWORD,LPVOID,WORD,IPINFO*,LPVOID,DWORD,DWORD))GetProcAddress(m_module,"IcmpSendEcho");

	if (m_pIcmpCreateFile == NULL || m_pIcmpCloseHandle == NULL	|| m_pIcmpSendEcho == NULL)
	{
		m_pIcmpCreateFile = NULL;
		m_pIcmpCloseHandle = NULL;
		m_pIcmpSendEcho = NULL;

		FreeLibrary(m_module);
		return;
	}

	m_initialised = true;
}

ICMPPing::~ICMPPing()
{
	if (m_module)
		FreeLibrary(m_module);
}

int ICMPPing::PingHost(const char *address)
{
	if (!m_initialised || !m_enabled)
		return -1;
	
	struct in_addr iaDest;		// Internet address structure

    iaDest.s_addr = inet_addr(address);

	HANDLE hndlFile = m_pIcmpCreateFile();

	IPINFO ipInfo;
	ipInfo.Ttl = 255;
	ipInfo.Tos = 0;
	ipInfo.IPFlags = 0;
	ipInfo.OptSize = 0;
	ipInfo.Options = NULL;

	ICMPECHO icmpEcho;
	DWORD dwRet = m_pIcmpSendEcho(hndlFile, iaDest.S_un.S_addr, NULL, 0, &ipInfo, &icmpEcho, sizeof(ICMPECHO), PING_TIMEOUT);

	int pingTime = -1;
	if (!icmpEcho.Status)
	{
		pingTime = icmpEcho.RTTime;
	}

	m_pIcmpCloseHandle(hndlFile);

	return pingTime;
}
