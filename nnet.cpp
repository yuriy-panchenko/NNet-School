#include "pch.h"
#include "nnet.h"

namespace nnet
{

	void net::init(std::initializer_list<int> const& topology)
	{
		assert(topology.size());
		auto iter{ topology.begin() };
		m_Input.resize(*iter);
		m_Layers.resize(topology.size());
		m_Error = .0;
		vd const* pInput{ &m_Input };

		for (auto& l : m_Layers)
		{
			l.init(pInput->size(), *iter++);
			pInput = &l;
		}
	}

	vd const& net::think(vd&& input)
	{
		assert(input.size() == m_Input.size());
		std::swap(m_Input, input);

		vd const* pInput{ &m_Input };

		for (auto& l : m_Layers)
		{
			l.think(*pInput);
			pInput = &l;
		}

		return *pInput;
	}

	double net::learn(vd const& real)
	{
		assert(!m_Layers.empty());

		auto iter{ m_Layers.rbegin() };
		{
			auto itInp{ std::next(iter) };
			cvd& inp{ itInp == m_Layers.rend() ? m_Input : *itInp };
			iter->set_real(inp.cbegin(), real);
		}

		for (iter = std::next(iter); iter != m_Layers.rend(); iter = std::next(iter))
		{
			auto itInp{ std::next(iter) };
			cvd& inp{ itInp == m_Layers.rend() ? m_Input : *itInp };
			iter->backpropagate(inp.cbegin(), *std::prev(iter));
		}

		for (auto& l : m_Layers)
			l.adjust();

		m_Error = .0;

		auto itReal{ real.begin() };
		for (auto out : m_Layers.back())
			m_Error += sq(out - *itReal++) / 2;

		//m_Error /= m_Layers.size();

		return m_Error;
	}

	double net::error()const
	{
		return m_Error;
	}
}