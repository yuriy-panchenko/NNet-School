
// NNet School.h : main header file for the NNet School application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CNNetSchoolApp:
// See NNet School.cpp for the implementation of this class
//

class CNNetSchoolApp : public CWinApp
{
public:
	CNNetSchoolApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CNNetSchoolApp theApp;
