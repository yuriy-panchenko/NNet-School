#pragma once

class MinMax
{
	double _Min, _Max;

public:
	MinMax() :_Min{ .0 }, _Max{ .0 } {}
	MinMax(double a, double b) :_Min{ min(a,b) }, _Max{ max(a,b) } {}

	void operator()(double val) { _Max = max(_Max, val), _Min = min(_Min, val); }
	bool IsFlat()const { return _Min == _Max; }
	double Spread()const { return _Max - _Min; }
	double Max()const { return _Max; }
};