#pragma once

class ScanResults
{
	size_t m_uTotal, m_uCorrect, m_uSample;
	CString m_strInfo;
	double m_dbGuessedMax;

public:
	ScanResults()=default;
	ScanResults(size_t total);
	size_t PickNextSample();
	size_t GetCurrentSample()const;
	void Add(BYTE bGuessed, BYTE bReal, double netErr, double outWin);
	CString const& GetInfo()const;
};