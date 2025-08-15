#pragma once
namespace nnet
{
	using vd = std::vector<double>;
	using cvd = std::vector<double> const;
	constexpr double
		cooficient_inertial{ .01 },
		cooficient_learning{ .001 };

	inline double derr(double out)
	{
		return 1. - sq(out);
	}

	inline double act(double val)
	{
		return tanh(val);
	}

	class acson
	{
		double m_W, m_dW;

	public:
		acson() :m_W{ rand() }, m_dW{ .0 } {}
		double think(double val)const { return val * m_W; }
		static double rand() { return  2. * ::rand() / RAND_MAX - 1.; }
		void backpropagate(double err)
		{
			m_dW *= cooficient_inertial;
			m_dW += cooficient_learning * err;
		}
		double weigth()const { return m_W; }
		void adjust() { m_W += m_dW; }
	};
}