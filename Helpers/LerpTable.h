#pragma once

#include "EnginePCH.h"

class CLerpTable
{
public:
	CLerpTable(float *pDataPairs, size_t nNumDataPairs)
	{
		for (size_t i = 0; i < nNumDataPairs; i++)
		{
			m_DataPairs.emplace_back(pDataPairs[i * 2 + 0], pDataPairs[i * 2 + 1]);
		}
	}

	void AddPair(float x, float y)
	{
		m_DataPairs.emplace_back(x, y);
	}

	float GetYVal(float x) const
	{
		if (0 == GetNumDataPairs())
		{
			return 0;
		}

		// если одна пара или ушёл в минимум, возвращаем первый
		if (1 == GetNumDataPairs() || x < GetX(0))
		{
			return GetY(0);
		}

		float x0 = GetX(0);
		float y0 = GetY(0);

		for (size_t i = 1; i < GetNumDataPairs(); i++)
		{
			const float x1 = GetX(i);
			const float y1 = GetY(i);

			// если в рамках - интерполируем и возвращаем
			if ((x >= x0) && (x < x1))
			{
				return (y0 + (y1 - y0) * (x - x0) / (x1 - x0));
			}

			x0 = x1;
			y0 = y1;
		}

		return GetY(GetNumDataPairs() - 1);
	}

	size_t GetNumDataPairs() const
	{
		return m_DataPairs.size();
	}

	float GetX(const size_t i) const
	{
		return m_DataPairs[i].first;
	}

	float GetY(const size_t i) const
	{
		return m_DataPairs[i].second;
	}

private:
	std::vector<std::pair<float, float>> m_DataPairs;
};