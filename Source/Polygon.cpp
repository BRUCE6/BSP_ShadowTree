
#include "Polygon.h"
#include "Utility.h"

#define MAXPTS 100

Point outpts[MAXPTS], inpts[MAXPTS];

Polygon::Polygon(const Point* points, int npoints) : Vertex(alloc_points(npoints)), NumVertices(npoints), _n(0, 0, 0)
{
	// Newell's Method
	for (int i = 0; i < npoints; ++i)
	{
		Vertex[i] = points[i];
		const Point& p1 = points[i], &p2 = points[(i + 1) % npoints];
		_n.x += (p1.y - p2.y) * (p1.z + p2.z);
		_n.y += (p1.z - p2.z) * (p1.x + p2.x);
		_n.z += (p1.x - p2.x) * (p1.y + p2.y);
	}
	_n = _n.normalize();
}

void Polygon::Split_Polygon(Plane part, Polygon*& front, Polygon*& back) const
{
	int count = NumVertices,
		out_c = 0,
		in_c = 0;
	Point ptA, ptB;
		
	int sideA, sideB;
	ptA = Vertex[count - 1];
	sideA = Utility::Classify_Point(part, ptA);
	for (int i = -1; ++i < count;)
	{
		ptB = Vertex[i];
		sideB = Utility::Classify_Point(part, ptB);
		if (sideB > 0)
		{
			if (sideA < 0)
			{
				Vec3 v = ptB - ptA;
				double sect = -Utility::Classify_Point(part, ptA) / part.n.dot(v);
				outpts[out_c++] = inpts[in_c++] = ptA + (v * sect);
			}
			outpts[out_c++] = ptB;
		}
		else if (sideB < 0)
		{
			if (sideA > 0)
			{
				Vec3 v = ptB - ptA;
				double sect = -Utility::Classify_Point(part, ptA) / part.n.dot(v);
				outpts[out_c++] = inpts[in_c++] = ptA + (v * sect);
			}
			inpts[in_c++] = ptB;
		}
		else
			outpts[out_c++] = inpts[in_c++] = ptB;
		ptA = ptB;
		sideA = sideB;
	}
	front = new Polygon(outpts, _n, out_c);
	back = new Polygon(inpts, _n, in_c);
}
