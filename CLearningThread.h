#pragma once
#include "nnet.h"
#define	WM_LEARN_SAMPLE	(WM_APP+0x0001)

// CLearningThread

class CLearningThread : public CWinThread
{
	DECLARE_DYNCREATE(CLearningThread)

protected:
	CLearningThread();           // protected constructor used by dynamic creation
	virtual ~CLearningThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	afx_msg void OnLearnSample(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

public:
	CWnd* m_pView;
	nnet::net m_Net;
	BYTE m_Guessed, m_Real;
	double m_outWin;

private:
	void Think(ImageFile::PICTURE const&);
	void Learn(BYTE);
};


