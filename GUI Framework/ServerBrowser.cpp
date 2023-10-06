// ServerBrowser.cpp : Defines the class behaviors for the application.
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
#include "ServerBrowserView.h"


#include "AboutBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServerBrowserApp

BEGIN_MESSAGE_MAP(CServerBrowserApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CServerBrowserApp construction

CServerBrowserApp::CServerBrowserApp()
{
	m_mainFrame = NULL;
}


// The one and only CServerBrowserApp object

CServerBrowserApp theApp;

// CServerBrowserApp initialization

#include <winsock2.h>
#include <ws2tcpip.h>


BOOL CServerBrowserApp::InitInstance()
{
	InitCommonControls();
	CWinApp::InitInstance();

	char filename[MAX_PATH];
	GetModuleFileName(NULL, filename, MAX_PATH);
	int fileNameLen = strlen(filename);
	for (int i=fileNameLen -1; i>0; i--)
	{
		if (filename[i] == '\\')
		{
			filename[i+1] = '\0';
			break;
		}
	}

	SetCurrentDirectory(filename);

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data))
	{
		AfxMessageBox("Unable to initialise Winsock");
		return FALSE;
	}

	SetRegistryKey(_T("Q4Max"));
	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CServerBrowserDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CServerBrowserView));

	if (!pDocTemplate)
		return FALSE;

	AddDocTemplate(pDocTemplate);
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.

	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it
	if (GetProfileInt("Positions", "IsMaximised", 1))
		m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	else
		m_pMainWnd->ShowWindow(SW_SHOW);

	m_pMainWnd->UpdateWindow();

	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}




// App command to run the dialog
void CServerBrowserApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CServerBrowserApp message handlers

// destroy
//	WSACleanup();