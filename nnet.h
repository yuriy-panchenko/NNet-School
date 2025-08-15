#pragma once
#include "layer.h"

namespace nnet
{
	class net
	{
	public:
		void init(std::initializer_list<int> const& topology);
		vd const& think(vd&&);
		double learn(vd const&);
		double error()const;

	private:
		vd m_Input;
		std::vector<layer> m_Layers;
		double m_Error;
	};
}