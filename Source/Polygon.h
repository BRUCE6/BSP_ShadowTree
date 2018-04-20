#pragma once

#include <math.h>
#include <iostream>

struct Vec3
{

	double x, y, z;

	Vec3() { x = 0; y = 0; z = 0; }
	Vec3(double x, double y, double z) : x(x), y(y), z(z) {}
	Vec3(const Vec3& copy) : x(copy.x), y(copy.y), z(copy.z) {}
	double length() const { return sqrt(x * x + y * y + z * z); }
	Vec3 normalize() const 
	{
		double l = length();
		return Vec3(x / l, y / l, z / l);
	}
	Vec3 operator* (const double& s) { return Vec3(x * s, y * s, z * s); }
	double dot(const Vec3& b) const { return x * b.x + y * b.y + z * b.z; }
	Vec3 cross(const Vec3& b) const { return Vec3(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x); }
};

struct Point : Vec3
{
	Point() {}
	~Point() {}
	Point(double x, double y, double z) : Vec3(x, y, z) {}
	Point(const Point& copy) : Vec3(copy) {}
	Vec3 operator- (const Point& b) const { return Vec3(x - b.x, y - b.y, z - b.z); }
	Point operator+ (const Vec3& v) const { return Point(x + v.x, y + v.y, z + v.z); }
};

struct Plane
{

	Vec3 n;
	double d;

	Plane(const Vec3& n, double d) : n(n), d(d) {}
	Plane() : n(0, 0, 0), d(0) {}
	
};

class Polygon
{
private:
	Point* Vertex;
	int NumVertices;
	Vec3 _n;
	static Point* alloc_points(int count) { return (Point*)malloc(sizeof(Point) * count); }
public:
	Polygon() : Vertex(NULL), NumVertices(0) {}
	Polygon(const Point& a, const Point& b, const Point& c) : Vertex(alloc_points(3)), NumVertices(3), _n(((b - a).cross(c - b)).normalize())
	{
		Vertex[0] = a;
		Vertex[1] = b;
		Vertex[2] = c;
	}
	Polygon(const Point* points, Vec3 n, int npoints)//compute intersection
	{
		Vertex = alloc_points(npoints);
		_n = n;
		NumVertices = npoints;
		for (int i = 0; i < npoints; i++)
			Vertex[i] = points[i];
	}
	Polygon(const Point* points, int npoints);

	Polygon(const Polygon& copy) : Vertex(alloc_points(copy.NumVertices)), NumVertices(copy.NumVertices), _n(copy._n)
	{
		for (int i = 0; i < copy.NumVertices; i++)
			Vertex[i] = copy.Vertex[i];
	}

	~Polygon() { free(Vertex); Vertex = NULL; }

	void operator=(const Polygon& other)
	{
		if (Vertex) free(Vertex);
		if (other.Size())
		{
			Vertex = alloc_points(other.NumVertices);
			NumVertices = other.NumVertices;
			for (int i = 0; i < other.NumVertices; ++i)
				Vertex[i] = other.Vertex[i];
		}
		else NumVertices = 0, Vertex = NULL;
		_n = other._n;
	}

	const Point& operator[](int index) const { return Vertex[index]; }
	int Size() const { return NumVertices; }
	Plane Get_Plane() const { return Plane(_n, -Vertex[0].dot(_n)); }
	void Split_Polygon(Plane part, Polygon*& front, Polygon*& back) const;
	const Vec3& normal() const { return _n; }
};

