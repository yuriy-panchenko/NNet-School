// CLearningThread.cpp : implementation file
//

#include "pch.h"
#include "NNet School.h"
#include "CLearningThread.h"
#include "ChildView.h"


// CLearningThread

IMPLEMENT_DYNCREATE(CLearningThread, CWinThread)

CLearningThread::CLearningThread()
{
}

CLearningThread::~CLearningThread()
{
}

BOOL CLearningThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CLearningThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

void CLearningThread::OnLearnSample(WPARAM, LPARAM)
{
	auto const sample{ static_cast<CChildView*>(m_pView)->GetNextSample() };
	m_Real = sample.val;
	Think(sample.pic);
	Learn(sample.val);
	m_pView->PostMessage(WM_SAMPLE_LEARNED);
}

void CLearningThread::Think(ImageFile::PICTURE const& pic)
{
	auto to_input = [&]()->std::vector<double>
		{
			std::vector<double> ret(28 * 28);
			auto iter{ pic };

			for (auto& px : ret)
			{
				px = *iter++ / 50.;
				if (px == .0)
					px = -1.;
			}

			return ret;
		};

	auto& out{ m_Net.think(to_input()) };
	auto me{ std::max_element(out.begin(), out.end()) };
	m_outWin = *me;
	m_Guessed = (BYTE)std::distance(out.begin(), me);
}

void CLearningThread::Learn(BYTE val)
{
	constexpr auto rx{ .7 };
	std::vector real(10, -rx);
	real[val] = rx;
	m_Net.learn(real);
}

BEGIN_MESSAGE_MAP(CLearningThread, CWinThread)
	ON_THREAD_MESSAGE(WM_LEARN_SAMPLE, OnLearnSample)
END_MESSAGE_MAP()
