#pragma once

#include <ctime>
#include <vector>
#include "Polygon.h"

class Utility
{
public:
	static double Classify_Point(const Plane& s, const Point& p)
	{
		Vec3 n = s.n;
		double d = s.d;
		return n.x * p.x + n.y * p.y + n.z * p.z + d;
	}
	static int Classify_Polygon(const Plane& s, const Polygon& poly)
	{
		double result = Classify_Point(s, poly[0]);
		for (int i = 1; i < poly.Size(); i++)
		{
			if (result != 0 && Classify_Point(s, poly[i]) * result < 0)
				return 3;
			else if (result == 0 && Classify_Point(s, poly[i]) != 0)
				result = Classify_Point(s, poly[i]);
		}
		if (result > 0)
			return 2;
		else if (result < 0)
			return 1;
		else
			return 0;
	}
	static void permute_list(std::vector<Polygon>& list)
	{
		srand((unsigned int) time(NULL));

		size_t len = list.size();
		for (size_t i = 0; i < len; i++)
		{
			size_t idx_a = (size_t)(rand() % len);
			size_t idx_b = (size_t)(rand() % len);

			Polygon temp = list[idx_a];
			list[idx_a] = list[idx_b];
			list[idx_b] = temp;
		}
	}
};
