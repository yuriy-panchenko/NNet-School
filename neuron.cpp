#include "pch.h"
#include "neuron.h"

namespace nnet
{
	void neuron::init(size_t acsons_count)
	{
		m_Bias = acson::rand();
		m_dBias = m_Error = .0;
		m_Acsons.resize(acsons_count);
	}

	double neuron::think(cvd& inp) const
	{
		auto ret{ m_Bias };
		auto iter{ inp.begin() };

		for (auto& a : m_Acsons)
			ret += a.think(*iter++);

		//ret /= m_Acsons.size()+1;
		ret /= sqrt(double(m_Acsons.size() + 1));

		return act(ret);
	}

	void neuron::backpropagate(cvd::const_iterator itInp, double err)
	{
		m_dBias *= cooficient_inertial;
		m_dBias += cooficient_learning * err;
		m_Error = err;

		for (auto& a : m_Acsons)
			a.backpropagate(*itInp++ * err);

		//adjust();
	}

	double neuron::weighted_error(size_t index) const
	{
		return m_Error * m_Acsons[index].weigth();
	}

	double neuron::error() const
	{
		return m_Error;
	}

	void neuron::adjust()
	{
		m_Bias += m_dBias;
		for (auto& a : m_Acsons)
			a.adjust();
	}
}
