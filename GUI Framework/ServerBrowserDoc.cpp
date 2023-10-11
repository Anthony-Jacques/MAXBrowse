// ServerBrowserDoc.cpp : implementation of the CServerBrowserDoc class
//
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
#include "ServerBrowser.h"

#include "MainFrm.h"
#include "ServerBrowserDoc.h"

#include "GameServer.h"

#include "ProgressDlg.h"
#include "ICMPPing.h"

#include <process.h>
#include <Mmsystem.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX_CONCURRENT_SERVER_QUERIES		20

// CServerBrowserDoc

IMPLEMENT_DYNCREATE(CServerBrowserDoc, CDocument)

BEGIN_MESSAGE_MAP(CServerBrowserDoc, CDocument)
END_MESSAGE_MAP()


// CServerBrowserDoc construction/destruction

CServerBrowserDoc::CServerBrowserDoc()
{
	m_currentGame = NULL;
	m_passwordFilter = true;
	m_punkbusterFilter = true;
	m_NoPunkbusterFilter = true;
	m_emptyFilter = true;
	m_fullFilter = true;
	m_tvFilter = false;

	m_cancelFlag = false;
	m_serversQueried = 0;

	LoadSupportedGames();

	InitializeCriticalSection(&m_queryCritSec);
}

CServerBrowserDoc::~CServerBrowserDoc()
{
	DeleteCriticalSection(&m_queryCritSec);

	m_supportedGames.DeleteContents(true);
}

bool CServerBrowserDoc::LoadSupportedGames()
{
	CFileFind finder;
	if (!finder.FindFile("SupportedGames.xml"))
	{
		AfxMessageBox("Failed to locate SupportedGames.xml - it should be alongside MAXBrowse.exe, but seems to be missing? No games can be configured in this state.");
		// to be nice, we could embed a default SupportedGames.xml, but...
		return false;
	}

	CoInitialize(NULL);

	bool success = true;

	MSXML2::IXMLDOMDocument2Ptr pXMLDom;
	HRESULT hr = pXMLDom.CreateInstance(__uuidof(DOMDocument));
	if (FAILED(hr)) 
	{
		AfxMessageBox("Failed to CreateInstance of an XML DOM.\nCheck msxml2 has been registered", MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
		success = false;
	}

	if (success)
	{
		// Set parser property settings
		pXMLDom->async = VARIANT_FALSE;
		
		hr = pXMLDom->load("SupportedGames.xml");
		
		if (hr != VARIANT_TRUE)
		{
			MSXML2::IXMLDOMParseErrorPtr pError = pXMLDom->parseError;

			long line = pError->Getline();
			long pos = pError->Getlinepos();

			_bstr_t parseError =
				_bstr_t("File: ") + _bstr_t(pError->Geturl()) + _bstr_t("\n\n") +
				_bstr_t("Line: ") + _bstr_t(pError->Getline()) + _bstr_t("\n") +
				_bstr_t("Pos:  ") + _bstr_t(pError->Getlinepos()) + _bstr_t("\n\n") +
				_bstr_t(pError->Getreason());

			MessageBox(NULL, parseError, "Parse Error", MB_OK);

			success = false;
		}
	}

	if (success)
	{
		pXMLDom->setProperty("SelectionLanguage", "XPath");

		MSXML2::IXMLDOMNodeListPtr pXMLDomNodeList = pXMLDom->documentElement->selectNodes(".//game");

		int count = pXMLDomNodeList->length;

		for (int i = 0; i < count; i++)
		{
			MSXML2::IXMLDOMNodePtr pGameNode = pXMLDomNodeList->Getitem(i);

			m_supportedGames.Append(new SupportedGame(pGameNode));
		}
	}

	if (pXMLDom)
		pXMLDom.Release();

	CoUninitialize();


	return success;
}

BOOL CServerBrowserDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	m_useICMPPing = theApp.GetProfileInt("Settings", "UseICMPPing", false);
	m_queryServerCount = theApp.GetProfileInt("Settings", "QueryServerCount", MAX_CONCURRENT_SERVER_QUERIES);
	if (m_queryServerCount > MAX_CONCURRENT_SERVER_QUERIES)
		m_queryServerCount = MAX_CONCURRENT_SERVER_QUERIES;
	else if (m_queryServerCount < 0)
		m_queryServerCount = 1;
	bool firstTimeSetup = theApp.GetProfileInt("Settings", "FirstTimeSetup", true);
	
	m_passwordFilter = theApp.GetProfileInt("Settings\\Filters", "Password", m_passwordFilter);
	m_punkbusterFilter = theApp.GetProfileInt("Settings\\Filters", "PunkBuster", m_punkbusterFilter);
	m_NoPunkbusterFilter = theApp.GetProfileInt("Settings\\Filters", "NoPunkBuster", m_NoPunkbusterFilter);
	m_emptyFilter = theApp.GetProfileInt("Settings\\Filters", "Empty", m_emptyFilter);
	m_fullFilter = theApp.GetProfileInt("Settings\\Filters", "Full", m_fullFilter);
	m_tvFilter = theApp.GetProfileInt("Settings\\Filters", "TV", m_tvFilter);

	m_nationalityMap.LoadIPDatabase();
	m_nationalityMap.CreateImageList();

	int numGames = m_supportedGames.Num();

	for (int i=0; i<numGames; i++)
	{
		m_supportedGames[i]->LoadConfigurationSettings();
		m_supportedGames[i]->GetMasterServer()->LoadServers();
	}

	idStr temp = theApp.GetProfileString("Settings", "CurrentGame", "Quake 4");
	SetGame(temp);

	if (!m_currentGame)
	{
		for (int i=0; i<m_supportedGames.Num(); i++)
		{
			SetGame(m_supportedGames[i]->GetName());
			if (m_currentGame)
				break;
		}
	}

	if (!m_currentGame || firstTimeSetup)
		theApp.m_mainFrame->PostMessage(WM_COMMAND, MAKELONG(ID_FILE_CONFIGUREGAMES, BN_CLICKED), 0);

//	theApp.m_mainFrame->PopulateGameMenu();

	m_selectedServer = NULL;

	FilterServerList();
	

	return TRUE;
}


void CServerBrowserDoc::PreCloseFrame(CFrameWnd* pFrame)
{
	CDocument::PreCloseFrame(pFrame);

	if (m_currentGame)
	{
		m_currentGame->GetMasterServer()->SaveServers();

		theApp.WriteProfileString("Settings", "CurrentGame", m_currentGame ? m_currentGame->GetName().c_str() : "");
	}

	theApp.WriteProfileInt("Settings", "UseICMPPing", m_useICMPPing);
	theApp.WriteProfileInt("Settings", "QueryServerCount", m_queryServerCount);
	theApp.WriteProfileInt("Settings", "FirstTimeSetup", 0);

	theApp.WriteProfileInt("Settings\\Filters", "Password", m_passwordFilter);
	theApp.WriteProfileInt("Settings\\Filters", "PunkBuster", m_punkbusterFilter);
	theApp.WriteProfileInt("Settings\\Filters", "NoPunkBuster", m_NoPunkbusterFilter);
	theApp.WriteProfileInt("Settings\\Filters", "Empty", m_emptyFilter);
	theApp.WriteProfileInt("Settings\\Filters", "Full", m_fullFilter);
	theApp.WriteProfileInt("Settings\\Filters", "TV", m_tvFilter);

}


// CServerBrowserDoc serialization

void CServerBrowserDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CServerBrowserDoc diagnostics

#ifdef _DEBUG
void CServerBrowserDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CServerBrowserDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CServerBrowserDoc commands


void CServerBrowserDoc::FilterServerList()
{
	m_filteredServers.Clear();

	if (!m_currentGame)
		return;

	const idList<GameServer*> &allServers = m_currentGame->GetMasterServer()->GetServers();

	CString protocolFilter;
	CString favouritesFilter;

	int numServers = allServers.Num();

	for (int i=0; i<numServers; i++)
	{
		bool matchesFilter = true;

		// first of all apply the filter-tree filters
		int numKeys = m_filter.GetNumKeyVals();
		for (int j=0; j<numKeys; j++)
		{
			const idKeyValue *thisPair = m_filter.GetKeyVal(j);
			
			if (idStr::ICmp(allServers[i]->GetServerInfo().GetString(thisPair->GetKey().c_str()), 
							thisPair->GetValue().c_str()) != 0)
			{
				matchesFilter = false;
				break;
			}
		}

		// and the special case ones
		if (!m_passwordFilter && allServers[i]->GetServerInfo().GetBool(m_currentGame->GetPasswordedCvar()))
			matchesFilter = false;

		if (!m_punkbusterFilter && !allServers[i]->GetServerInfo().GetBool(m_currentGame->GetPunkbusterCvar()))
			matchesFilter = false;

		if (!m_NoPunkbusterFilter && allServers[i]->GetServerInfo().GetBool(m_currentGame->GetPunkbusterCvar()))
			matchesFilter = false;

		if (!m_emptyFilter && allServers[i]->GetNumPlayers() == 0)
			matchesFilter = false;

		if (!m_fullFilter && (allServers[i]->GetNumPlayers() == allServers[i]->GetServerInfo().GetInt(m_currentGame->GetMaxPlayersCvar())))
			matchesFilter = false;

		if (m_currentGame->GetFavouritesFilter().Length() > 0 && !allServers[i]->IsFavourite(m_currentGame->GetFavouritesFilter().c_str()))
			matchesFilter = false;

		if (m_currentGame->GetProtocolFilter().Length() > 0 && idStr::ICmp(allServers[i]->GetServerInfo().GetString("protocol"), m_currentGame->GetProtocolFilter().c_str()) != 0)
			matchesFilter = false;

		if (matchesFilter)
			matchesFilter = (m_tvFilter == allServers[i]->IsTV());

		if (matchesFilter)
			m_filteredServers.Append(allServers[i]);
	}
}

SupportedGame *CServerBrowserDoc::GetGame(const idStr &gameName)
{
	int numGames = m_supportedGames.Num();

	for (int i=0; i<numGames; i++)
	{
		if (gameName.ICmp(m_supportedGames[i]->GetName().c_str()) == 0)
			return m_supportedGames[i];
	}

	return NULL;
}

void CServerBrowserDoc::SetGame(const idStr &newGame)
{
	SupportedGame *thisGame = GetGame(newGame);

	if (!thisGame || !thisGame->IsInstalled())
	{
		m_currentGame = NULL;
		SetTitle("");
		FilterServerList();
	}
	else
	{
		if (m_currentGame)
			m_currentGame->GetMasterServer()->SaveServers();

		m_currentGame = thisGame;

		SetTitle(m_currentGame->GetName().c_str());

		m_selectedServer = NULL;

		MasterServer *currentMaster = m_currentGame->GetMasterServer();
		if (currentMaster->GetServers().Num() == 0)
		{
			if (!currentMaster->LoadServers())
				CheckForNewServers(false);
		}

		UpdateAllViews(NULL, VIEW_GAME_CHANGED);
	}
}


void CServerBrowserDoc::QueryOutstandingServers()
{
	EnterCriticalSection(&m_queryCritSec);

	bool isEmpty = (m_serversToQuery.Num() == 0);

	while (!isEmpty)
	{
		GameServer *serverToQuery = m_serversToQuery[0];
		m_serversToQuery.RemoveIndex(0);

		LeaveCriticalSection(&m_queryCritSec);

		serverToQuery->Query();

		EnterCriticalSection(&m_queryCritSec);

		m_serversQueried++;
		isEmpty = (m_serversToQuery.Num() == 0) || m_cancelFlag;
	}

	LeaveCriticalSection(&m_queryCritSec);
}

void CServerBrowserDoc::AddServerToQuery(GameServer *thisServer)
{
	EnterCriticalSection(&m_queryCritSec);

	m_serversToQuery.Append(thisServer);

	LeaveCriticalSection(&m_queryCritSec);
}


void CServerBrowserDoc::QueryServers(CWnd *parentWnd)
{
	if (parentWnd == NULL)
		parentWnd = AfxGetMainWnd();

	m_serversQueried = 0;
	m_cancelFlag = false;

	CWaitCursor waitCursor;
	CProgressDlg progressDlg(parentWnd, &m_serversQueried, m_serversToQuery.Num(), &m_cancelFlag);

	progressDlg.Create(CProgressDlg::IDD, parentWnd);
	progressDlg.ShowWindow(SW_SHOW);

	ICMPPing::GetInstance().SetEnabled(m_useICMPPing);

    HANDLE threadHandles[MAX_CONCURRENT_SERVER_QUERIES];
    HANDLE openedThreadHandles[MAX_CONCURRENT_SERVER_QUERIES];
	int numThreadsRunning = 0;
	
	timeBeginPeriod(1);

	for (int i=0; i<min(m_queryServerCount, m_serversToQuery.Num()); i++)
	{
		threadHandles[i] = (HANDLE)_beginthreadex(NULL, 0, CServerBrowserDoc::ServerQueryThread, (void*)this, 0, NULL);
		SetThreadPriority(threadHandles[i], THREAD_PRIORITY_ABOVE_NORMAL);

		openedThreadHandles[i] = threadHandles[i];
		numThreadsRunning++;
	}

	int numThreadsStarted = numThreadsRunning;
	DWORD lWaitObject = MsgWaitForMultipleObjects(numThreadsStarted, threadHandles, FALSE, INFINITE, QS_ALLINPUT);
	while (numThreadsRunning > 0)
	{
		if (lWaitObject == (WAIT_OBJECT_0 + numThreadsRunning))
		{
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == TRUE)
			{
				if ((msg.message < WM_MOUSEFIRST || WM_MOUSELAST < msg.message) &&
					(msg.message < WM_KEYFIRST   || WM_KEYLAST < msg.message))
				{
					TranslateMessage(&msg) ;
					DispatchMessage(&msg) ; 
				}
			}
		}
		else 
		{
			ASSERT(lWaitObject >= WAIT_OBJECT_0 && lWaitObject < (WAIT_OBJECT_0 + numThreadsRunning));

			numThreadsRunning--;

			// shift all the thread handles down so that we dont fire on the threads that have already completed
			int threadNum = lWaitObject - WAIT_OBJECT_0;
			for (int i=threadNum; i<MAX_CONCURRENT_SERVER_QUERIES - 1; i++)
			{
				threadHandles[i] = threadHandles[i + 1];
			}
		}

		if (numThreadsRunning > 0)
			lWaitObject = MsgWaitForMultipleObjects(numThreadsRunning, threadHandles, FALSE, INFINITE, QS_ALLINPUT);
	}

	timeEndPeriod(1);

	for (int i=0; i<numThreadsStarted; i++)
		CloseHandle(openedThreadHandles[i]);

	EnterCriticalSection(&m_queryCritSec);
	m_serversToQuery.Clear();
	LeaveCriticalSection(&m_queryCritSec);

	FilterServerList();

	progressDlg.DestroyWindow();
}


unsigned int CServerBrowserDoc::ServerQueryThread(void *params)
{
	CServerBrowserDoc *theDocument = (CServerBrowserDoc*)params;

	theDocument->QueryOutstandingServers();

	_endthreadex(0);

	return 0;
}


unsigned int CServerBrowserDoc::MasterQueryThread(void *params)
{
	CServerBrowserDoc *theDocument = (CServerBrowserDoc*)params;

	theDocument->GetCurrentGame()->GetMasterServer()->Query(0);

	_endthreadex(0);

	return 0;
}
	

void CServerBrowserDoc::CheckForNewServers(bool requeryExistingServers, CWnd *parentWnd)
{
	if (parentWnd == NULL)
		parentWnd = AfxGetMainWnd();

	m_serversQueried = 0;
	m_cancelFlag = false;

	CWaitCursor waitCursor;
	CProgressDlg progressDlg(parentWnd, &m_serversQueried, 0, &m_cancelFlag, "Contacting master server...");

	progressDlg.Create(CProgressDlg::IDD, parentWnd);
	progressDlg.ShowWindow(SW_SHOW);
	
	HANDLE masterQueryThread = (HANDLE)_beginthreadex(NULL, 0, CServerBrowserDoc::MasterQueryThread, (void*)this, 0, NULL);

	DWORD lWaitObject = MsgWaitForMultipleObjects(1, &masterQueryThread, FALSE, INFINITE, QS_ALLINPUT);
	while (lWaitObject != WAIT_OBJECT_0)
	{
		if (lWaitObject == (WAIT_OBJECT_0 + 1))
		{
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == TRUE)
			{
				if ((msg.message < WM_MOUSEFIRST || WM_MOUSELAST < msg.message) &&
					(msg.message < WM_KEYFIRST   || WM_KEYLAST < msg.message))
				{
					TranslateMessage(&msg) ;
					DispatchMessage(&msg) ; 
				}
			}
		}

		lWaitObject = MsgWaitForMultipleObjects(1, &masterQueryThread, FALSE, INFINITE, QS_ALLINPUT);
	}

	CloseHandle(masterQueryThread);
	progressDlg.DestroyWindow();

	const idList<GameServer*> &servers = requeryExistingServers ? 
									m_currentGame->GetMasterServer()->GetServers() 
								:	m_currentGame->GetMasterServer()->GetNewServers();

	int numServers = servers.Num();

	for (int i=0; i<numServers; i++)
		AddServerToQuery(servers[i]);

	if (numServers)
	{
		QueryServers();
		UpdateAllViews(NULL, SERVERS_REFRESHED);
	}

	m_currentGame->GetMasterServer()->ClearNewServersList();

}

void CServerBrowserDoc::ImportIPs(CString ipList)
{
	int count = m_currentGame->GetMasterServer()->ImportIPs(ipList);
	if (count > 0)
	{
		const idList<GameServer*> &servers = m_currentGame->GetMasterServer()->GetNewServers();
		int numServers = servers.Num();

		for (int i=0; i<numServers; i++)
			AddServerToQuery(servers[i]);

		QueryServers();
		UpdateAllViews(NULL, SERVERS_REFRESHED);

		m_currentGame->GetMasterServer()->ClearNewServersList();

		CString message;
		message.Format("Successfully imported %d servers", numServers);
		AfxMessageBox(message);
	}
	else
	{
		AfxMessageBox("No new servers were found");
	}
}

void CServerBrowserDoc::DeleteOldServers()
{
	m_currentGame->GetMasterServer()->DeleteOldServers();

	FilterServerList();

	UpdateAllViews(NULL, SERVERS_REFRESHED);
}