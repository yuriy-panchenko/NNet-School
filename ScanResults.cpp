#include "pch.h"
#include "ScanResults.h"

ScanResults::ScanResults(size_t total)
	:m_uTotal{total }
	,m_uCorrect{}
	, m_uSample{}
	, m_dbGuessedMax{ }
{
}

size_t ScanResults::PickNextSample()
{
	return m_uSample++;
}

size_t ScanResults::GetCurrentSample() const
{
	return m_uSample;
}

void ScanResults::Add(BYTE bGuessed, BYTE bReal,double netErr,double outWin)
{
	if (bGuessed == bReal)
		++m_uCorrect;

	if (m_uSample > 99ull)
		m_dbGuessedMax = max(m_dbGuessedMax, m_uCorrect * 100. / m_uSample);

	m_strInfo.Format(_T("Learned %I64u / %I64u, correct = %I64u"
		"\nError = %.6f, %.2f%%, max: %.2f%%"
		"\nReal = %d, Guessed = %d"
		"\nout win = %.5f"),
		m_uSample,
		m_uTotal,
		m_uCorrect,
		netErr,
		m_uCorrect * 100. / m_uSample,
		m_dbGuessedMax,
		bReal,
		bGuessed,
		outWin
	);

}

CString const& ScanResults::GetInfo() const
{
	return m_strInfo;
}
