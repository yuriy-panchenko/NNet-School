#include "pch.h"
#include "layer.h"

namespace nnet
{

	void layer::init(size_t acsons, size_t cells)
	{
		resize(cells);
		m_Cells.resize(cells);

		for (auto& n : m_Cells)
			n.init(acsons);
	}

	void layer::think(cvd& input)
	{
		auto itOut{ begin() };

		for (auto& n : m_Cells)
			*itOut++ = n.think(input);
	}

	void layer::set_real(cvd::const_iterator itInp, cvd& real)
	{
		assert(real.size() == size());

		auto itReal{ real.begin() };
		auto itOut{ begin() };

		for (auto& n : m_Cells)
		{
			n.backpropagate(itInp, (*itReal - *itOut) * derr(*itOut));
			++itReal, ++itOut;
		}
	}

	void layer::backpropagate(cvd::const_iterator itInp, layer const& nextLayer)
	{
		auto itOut{ begin() };
		size_t i{ 0ull };

		for (auto& n : m_Cells)
			n.backpropagate(itInp, nextLayer.weighted_error(i++) * derr(*itOut++));
	}

	double layer::weighted_error(size_t index) const
	{
		double ret{ .0 };

		for (auto& n : m_Cells)
			ret += n.weighted_error(index);

		return ret;// m_Cells.size();
	}

	void layer::adjust()
	{
		for (auto& n : m_Cells)
			n.adjust();
	}
}