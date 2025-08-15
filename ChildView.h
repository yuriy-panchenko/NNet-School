
// ChildView.h : interface of the CChildView class
//
#define WM_SAMPLE_LEARNED	(WM_USER+0x0001)

#pragma once
#include "CLearningThread.h"

// CChildView window

class CChildView : public CWnd
{
	// Construction
public:
	CChildView();
	virtual ~CChildView();

	ImageSet::Data GetNextSample();

	// Attributes
public:

	// Operations
public:

	// Overrides
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// Implementation
public:

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	afx_msg void OnLearnNextSample();
	afx_msg void OnUpdateLearnNextSample(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnSampleLearned(WPARAM,LPARAM);
	DECLARE_MESSAGE_MAP()

private:
	void DrawGrayscaleImage(CDC& dc, int x, int y, int drawWidth, int drawHeight, const unsigned char* pixels);

private:
	ImageSet m_imgSet;
	std::unique_ptr<std::byte[]> m_bmpInfo;
	CLearningThread* m_pTh;
	int m_iSample;
	size_t m_iCorrect;
	CString m_strInfo;
	double m_dbGuessedMax;
};

