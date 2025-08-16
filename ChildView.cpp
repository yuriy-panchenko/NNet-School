
// ChildView.cpp : implementation of the CChildView class
//

#include "pch.h"
#include "framework.h"
#include "NNet School.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SCALE_WIDTH		100

constexpr auto
//FILEPATH_IMAGE{_T("res/t10k-images.idx3-ubyte")},
FILEPATH_IMAGE{ _T("res/train-images.idx3-ubyte") },
FILEPATH_LABEL{ _T("res/train-labels.idx1-ubyte") };
//FILEPATH_LABEL{_T("res/t10k-labels.idx1-ubyte")};

// CChildView

CChildView::CChildView()
	:m_imgSet{ FILEPATH_IMAGE, FILEPATH_LABEL }
	, m_pTh{ NULL }
{
	::srand(10);

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

	LOGFONT lf{};
	lf.lfHeight = 18;
	lf.lfWeight = FW_NORMAL;
	wcscpy_s(lf.lfFaceName, _T("Terminal"));
	m_fontInfo.CreateFontIndirect(&lf);
}

CChildView::~CChildView()
{
}

ImageSet::Data CChildView::GetNextSample()
{
	auto const iSample{ m_ScanResults.PickNextSample() };
	ASSERT(iSample < (int)m_imgSet.GetCount());
	return m_imgSet.GetSample(iSample);
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_COMMAND(ID_LEARN_NEXT_SAMPLE, &CChildView::OnLearnNextSample)
	ON_UPDATE_COMMAND_UI(ID_LEARN_NEXT_SAMPLE, &CChildView::OnUpdateLearnNextSample)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_SAMPLE_LEARNED, OnSampleLearned)
	ON_WM_ERASEBKGND()
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

	CRect canvas;
	GetClientRect(canvas);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, canvas.Width(), canvas.Height());
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	int const iSave{ memDC.SaveDC() };
	memDC.SelectObject(bmp);

	auto rect{ canvas };
	rect.bottom -= 300;
	rect.right -= SCALE_WIDTH;
	rect.DeflateRect(0, 20, 0, 20);
	m_ScanResults.DrawGuessed(memDC, rect);
	rect.left = rect.right;
	rect.right = canvas.right;
	DrawScale(memDC, rect, m_ScanResults.GetGuessedMinMax());


	rect = canvas;
	rect.top = canvas.bottom - 300;
	rect.DeflateRect(0, 20, SCALE_WIDTH, 20);
	m_ScanResults.DrawErrors(memDC, rect);


	rect = canvas;
	rect.DeflateRect(20, 20, 0, 20);
	memDC.SetTextColor(RGB(20, 220, 20));
	memDC.SetBkMode(TRANSPARENT);
	memDC.SelectObject(m_fontInfo);
	memDC.DrawText(m_ScanResults.GetInfo(), rect, DT_LEFT);
	//DrawGrayscaleImage(dc, 10, 10, 28, 28, *m_imgSet.GetPictures().begin());

	dc.BitBlt(0, 0, canvas.Width(), canvas.Height(), &memDC, 0, 0, SRCCOPY);
	memDC.RestoreDC(iSave);
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

void CChildView::DrawScale(CDC&, CRect const&, MinMax)
{
}

void CChildView::OnLearnNextSample()
{
	if (!m_pTh)
	{
		m_ScanResults = { m_imgSet.GetCount() };

		m_pTh = (CLearningThread*)::AfxBeginThread(RUNTIME_CLASS(CLearningThread), 0, 0, CREATE_SUSPENDED);
		m_pTh->m_pView = this;
		m_pTh->m_Net.init({
			28 * 28,
			28 * 28,
			28 * 28,
			//28 * 28,
			//20,
			//28 * 28 / 2,
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
	m_ScanResults.Add(m_pTh->m_Guessed, m_pTh->m_Real, m_pTh->m_Net.error(), m_pTh->m_outWin);

	Invalidate();

	if (m_ScanResults.GetCurrentSample() < (int)m_imgSet.GetCount())
		m_pTh->PostThreadMessage(WM_LEARN_SAMPLE, 0, 0);

	return 0;
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	//return CWnd::OnEraseBkgnd(pDC);
	return TRUE;
}
