#pragma once

class ScanResults
{
	size_t m_uTotal, m_uCorrect, m_uSample;
	CString m_strInfo;
	double m_dbGuessedMax, m_dbGuesThouMax;
	MinMax m_mmGuessed, m_mmErrors;
	std::vector<std::pair<BYTE, BYTE>> m_QA;
	std::vector<double> m_Error, m_WinOut, m_Guessed, m_GuesThou;

public:
	ScanResults() = default;
	ScanResults(size_t total);
	size_t PickNextSample();
	size_t GetCurrentSample()const;
	void Add(BYTE bGuessed, BYTE bReal, double netErr, double outWin);
	CString const& GetInfo()const;
	void DrawGuessed(CDC& dc, CRect const& canvas);
	void DrawErrors(CDC& dc, CRect const& canvas);
	MinMax GetGuessedMinMax()const;

private:
	void DrawLine(CDC&, CRect const& canvas,
		std::vector<double>::const_iterator itFrom,
		std::vector<double>::const_iterator itEnd,
		MinMax,
		COLORREF);
};