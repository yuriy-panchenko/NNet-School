#include "pch.h"
#include "ScanResults.h"

ScanResults::ScanResults(size_t total)
	:m_uTotal{ total }
	, m_uCorrect{}
	, m_uSample{}
	, m_dbGuessedMax{ }
	, m_dbGuesThouMax{}
	, m_mmGuessed{}
	, m_mmErrors{}
{
	m_QA.reserve(total);
	m_Error.reserve(total);
	m_WinOut.reserve(total);
	m_Guessed.reserve(total);
	m_GuesThou.reserve(total);
}

size_t ScanResults::PickNextSample()
{
	return m_uSample++;
}

size_t ScanResults::GetCurrentSample() const
{
	return m_uSample;
}

void ScanResults::Add(BYTE bGuessed, BYTE bReal, double netErr, double outWin)
{
	m_QA.emplace_back(bGuessed, bReal);
	m_Error.push_back(netErr);
	m_WinOut.push_back(outWin);
	m_Guessed.push_back(m_uCorrect * 100. / m_uSample);

	auto calc_thou = [](auto itFrom, auto itTo)->double
		{
			auto const count{ std::distance(itFrom,itTo) };
			if (!count)
				return .0;

			size_t right{};

			for (auto iter{ itFrom }; iter != itTo; iter++)
				if (iter->first == iter->second)
					++right;

			return right * 100. / count;
		};

	m_GuesThou.push_back(calc_thou(m_QA.size() >= 1'000 ? m_QA.end() - 1'000 : m_QA.begin(), m_QA.end()));

	//m_GuesThou.push_back();

	if (bGuessed == bReal)
		++m_uCorrect;

	if (m_uSample > 99ull)
	{
		m_dbGuessedMax = max(m_dbGuessedMax, m_Guessed.back());
		m_dbGuesThouMax = max(m_dbGuesThouMax, m_GuesThou.back());
	}

	m_strInfo.Format(_T("%I64u / %I64u / %I64u"
		"\n1000 %.2f%%, max: %.2f%%"
		"\nALL   %.2f%%, max: %.2f%%"
		"\nReal = %d, Guessed = %d"
		"\nError = %.6f"
		"\nout win = %.5f"),
		m_uCorrect, m_uSample, m_uTotal,
		m_GuesThou.back(), m_dbGuesThouMax,
		m_Guessed.back(), m_dbGuessedMax,
		bReal, bGuessed,
		netErr,
		outWin
	);

}

CString const& ScanResults::GetInfo() const
{
	return m_strInfo;
}

void ScanResults::DrawGuessed(CDC& dc, CRect const& canvas)
{
	auto bars{ min(m_Guessed.size(), (size_t)canvas.Width()) };
	if (bars > 1)
	{

		m_mmGuessed =
		{
			min(*std::min_element(m_Guessed.end() - bars, m_Guessed.end()), *std::min_element(m_GuesThou.end() - bars, m_GuesThou.end())), 
			max(*std::max_element(m_Guessed.end() - bars, m_Guessed.end()),*std::max_element(m_GuesThou.end() - bars, m_GuesThou.end())) 
		};

		m_mmGuessed(m_dbGuesThouMax);

		DrawLine(dc, canvas, m_GuesThou.end() - bars, m_GuesThou.end(), m_mmGuessed, RGB(30, 180, 180));
		DrawLine(dc, canvas, m_Guessed.end() - bars, m_Guessed.end(), m_mmGuessed, RGB(180, 180, 30));
	}
}

void ScanResults::DrawErrors(CDC& dc, CRect const& canvas)
{
	auto bars{ min(m_Error.size(), (size_t)canvas.Width()) };
	if (bars > 1)
	{
		m_mmErrors =
		{
			min(*std::min_element(m_Error.end() - bars, m_Error.end()), *std::min_element(m_WinOut.end() - bars, m_WinOut.end())),
			max(*std::max_element(m_Error.end() - bars, m_Error.end()),*std::max_element(m_WinOut.end() - bars, m_WinOut.end()))
		};

		DrawLine(dc, canvas, m_WinOut.end() - bars, m_WinOut.end(), m_mmErrors, RGB(30, 30, 180));
		DrawLine(dc, canvas, m_Error.end() - bars, m_Error.end(), m_mmErrors, RGB(180, 30, 30));
	}
}

MinMax ScanResults::GetGuessedMinMax() const
{
	return m_mmGuessed;
}

void ScanResults::DrawLine(CDC& dc, CRect const& canvas,
	std::vector<double>::const_iterator itFrom,
	std::vector<double>::const_iterator itEnd,
	MinMax mm, COLORREF col)
{
	if (mm.IsFlat())
		mm = { -1., 1. };
	auto const iSave{ dc.SaveDC() };
	CPen pen{ PS_SOLID, 1, col };
	dc.SelectObject(pen);

	auto const PpP{ canvas.Height() /mm.Spread()};

	auto to_screen_y = [&](double val)->int
		{
			return canvas.top + (int)((mm.Max() - val) * PpP + .5);
		};

	int x{ canvas.left }, y{ to_screen_y(*itFrom) };
	dc.MoveTo(x, y);

	for (auto iter{ std::next(itFrom) }; iter != itEnd; iter++)
	{
		y = to_screen_y(*iter);
		dc.LineTo(x++, y);
		dc.LineTo(x, y);
	}

	dc.RestoreDC(iSave);
}
