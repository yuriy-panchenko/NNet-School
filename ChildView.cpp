
// ChildView.cpp : implementation of the CChildView class
//

#include "pch.h"
#include "framework.h"
#include "NNet School.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

constexpr auto
//FILEPATH_IMAGE{_T("res/t10k-images.idx3-ubyte")},
FILEPATH_IMAGE{ _T("res/train-images.idx3-ubyte") },
FILEPATH_LABEL{ _T("res/train-labels.idx1-ubyte") };
//FILEPATH_LABEL{_T("res/t10k-labels.idx1-ubyte")};

// CChildView

CChildView::CChildView()
	:m_imgSet{ FILEPATH_IMAGE, FILEPATH_LABEL }
	, m_pTh{ NULL }
	, m_iSample{ 0 }
{
	int32_t const bmi_size{ sizeof BITMAPINFOHEADER + sizeof RGBQUAD * 256 };
	m_bmpInfo = std::make_unique<std::byte[]>(bmi_size);
	auto& bmi{ *reinterpret_cast<BITMAPINFO*>(m_bmpInfo.get()) };

	ZeroMemory(&bmi, bmi_size);

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = 28;
	bmi.bmiHeader.biHeight = -28; // top-down
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 8;       // 8-bit grayscale
	bmi.bmiHeader.biCompression = BI_RGB;

	// Fill grayscale palette
	for (int i = 0; i < 256; i++)
		bmi.bmiColors[i].rgbRed = bmi.bmiColors[i].rgbGreen = bmi.bmiColors[i].rgbBlue = i;
}

CChildView::~CChildView()
{
}

ImageSet::Data CChildView::GetNextSample()
{
	ASSERT(m_iSample < (int)m_imgSet.GetCount());
	return m_imgSet.GetSample(m_iSample++);
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_COMMAND(ID_LEARN_NEXT_SAMPLE, &CChildView::OnLearnNextSample)
	ON_UPDATE_COMMAND_UI(ID_LEARN_NEXT_SAMPLE, &CChildView::OnUpdateLearnNextSample)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_SAMPLE_LEARNED, OnSampleLearned)
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), nullptr);

	return TRUE;
}

void CChildView::OnPaint()
{
	CPaintDC dc{ this }; // device context for painting

	CRect rect;
	GetClientRect(rect);
	dc.DrawText(m_strInfo, rect, DT_LEFT);
	//DrawGrayscaleImage(dc, 10, 10, 28, 28, *m_imgSet.GetPictures().begin());
}

void CChildView::DrawGrayscaleImage(CDC& dc, int x, int y, int drawWidth, int drawHeight, const unsigned char* pixels)
{
	auto& bmi{ *reinterpret_cast<BITMAPINFO const*>(m_bmpInfo.get()) };
	StretchDIBits(
		dc,
		x,
		y,
		drawWidth,
		drawHeight, // destination rectangle
		0,
		0,
		bmi.bmiHeader.biWidth,
		abs(bmi.bmiHeader.biHeight),
		pixels,
		&bmi,
		DIB_RGB_COLORS,
		SRCCOPY
	);
}

void CChildView::OnLearnNextSample()
{
	if (!m_pTh)
	{
		m_iSample = 0;
		m_dbGuessedMax = .0;
		m_iCorrect = 0ull;
		::srand(10);

		m_pTh = (CLearningThread*)::AfxBeginThread(RUNTIME_CLASS(CLearningThread), 0, 0, CREATE_SUSPENDED);
		m_pTh->m_pView = this;
		m_pTh->m_Net.init({
			28 * 28,
			28 * 28,
			//28 * 28 /2,
			//28 * 28 / 4,
			//28 * 28 / 8,
			10 });
		m_pTh->ResumeThread();
	}
	m_pTh->PostThreadMessage(WM_LEARN_SAMPLE, 0, 0);
}

void CChildView::OnUpdateLearnNextSample(CCmdUI* pCmdUI)
{
}

void CChildView::OnDestroy()
{
	CWnd::OnDestroy();

	if (m_pTh)
	{
		m_pTh->PostThreadMessage(WM_QUIT, 0, 0);
		::WaitForSingleObject(*m_pTh, INFINITE);
		m_pTh = NULL;
	}
}

LRESULT CChildView::OnSampleLearned(WPARAM, LPARAM)
{
	if (m_pTh->m_Guessed == m_pTh->m_Real)
		++m_iCorrect;

	if (m_iSample > 99)
		m_dbGuessedMax = max(m_dbGuessedMax, m_iCorrect * 100. / m_iSample);

	m_strInfo.Format(_T("Learned %d / %I64u, correct = %I64u"
		"\nError = %.6f, %.2f%%, max: %.2f%%"
		"\nReal = %d, Guessed = %d"
		"\nout win = %.5f"),
		m_iSample,
		m_imgSet.GetCount(),
		m_iCorrect,
		m_pTh->m_Net.error(),
		m_iCorrect * 100. / m_iSample,
		m_dbGuessedMax,
		m_pTh->m_Real,
		m_pTh->m_Guessed,
		m_pTh->m_outWin
	);

	Invalidate();

	if (m_iSample < (int)m_imgSet.GetCount())
		m_pTh->PostThreadMessage(WM_LEARN_SAMPLE, 0, 0);

	return 0;
}
