#pragma once

#include <vector>
#include <GL/glut.h>

#include "Polygon.h"
class BSP_tree
{
private:
	Plane partition;
	std::vector<Polygon> polygons;
	BSP_tree *front, *back;

	//for shadow
	bool is_out;
public:
	enum PositionType {
		COINCIDENT,
		IN_BACK_OF,
		IN_FRONT_OF,
		SPAN
	};
	BSP_tree() {};
	BSP_tree(bool is_out) : is_out(is_out) 
	{
		front = NULL;
		back = NULL;
	}
	~BSP_tree() 
	{ 
		polygons.clear(); 
		delete(front); 
		delete(back); 
		front = NULL;
		back = NULL;
	}
	void Build_BSP_Tree(std::vector<Polygon> list);
	void Draw_Polygon_List(int type);
	void Draw_BSP_Tree(Point eye, int type);
	bool is_leaf() { return (front == NULL && back == NULL); }
	//for shadow
	void Add_Polygon(const Polygon& polygon);
	void Shadow(const Point& light, const Polygon& polygon, std::vector<Polygon>*& lit_polygons, std::vector<Polygon>*& shadow_polygons);
	void Draw_Shadow(BSP_tree*& ShadowBSP, const Point& light, std::vector<Polygon>*& lit_polygons, std::vector<Polygon>*& shadow_polygons);
	void near_to_far(const Point& light, std::vector<Polygon>& sort_list);
};