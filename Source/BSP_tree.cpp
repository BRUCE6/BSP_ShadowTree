#include <vector>

#include "Polygon.h"
#include "BSP_tree.h"
#include "Utility.h"

using namespace std;

void BSP_tree::Build_BSP_Tree(vector<Polygon> list)
{
	if (list.size() == 0) return;
	vector<Polygon>::iterator iter = list.begin();
	Polygon root = *iter++;
	partition = root.Get_Plane();
	polygons.push_back(root);
	vector<Polygon> front_list, back_list;
	//Polygon poly;
	while (iter != list.end())
	{
		Polygon poly = Polygon(*iter++); //very important for destruct
		int result = Utility::Classify_Polygon(partition, poly);

		switch (result)
		{
		case COINCIDENT:
			polygons.push_back(poly);
			break;
		case IN_BACK_OF:
			back_list.push_back(poly);
			break;
		case IN_FRONT_OF:
			front_list.push_back(poly);
			break;
		case SPAN:
			Polygon *front_piece, *back_piece;
			poly.Split_Polygon(partition, front_piece, back_piece);
			back_list.push_back(*back_piece);
			front_list.push_back(*front_piece);
			break;
		default:
			break;
		}
	}
	if (!front_list.empty())
	{
		front = new BSP_tree;
		front->Build_BSP_Tree(front_list);
	}
	if (!back_list.empty())
	{
		back = new BSP_tree;
		back->Build_BSP_Tree(back_list);
	}
	front_list.clear();
	back_list.clear();
}

void BSP_tree::Draw_Polygon_List(int type)
{
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < polygons.size(); i++)
	{
		Polygon fragment = polygons[i];
		if (type == 0)
			glColor3f(1, 0, 0);
		else
			glColor3f(1, 1, 0);
		glNormal3f(fragment.normal().x, fragment.normal().y, fragment.normal().z);
		for (int i = 1; i < fragment.Size() - 1; i++)
		{
			glVertex3f(fragment[0].x, fragment[0].y, fragment[0].z);
			glVertex3f(fragment[i].x, fragment[i].y, fragment[i].z);
			glVertex3f(fragment[i + 1].x, fragment[i + 1].y, fragment[i + 1].z);
		}
	}
	glEnd();

	glColor3f(0, 0, 0);
	for (int i = 0; i < polygons.size(); ++i)
	{
		Polygon fragment = polygons[i];
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < fragment.Size(); ++i)
			glVertex3f(fragment[i].x, fragment[i].y, fragment[i].z);
		glEnd();
	}
}

void BSP_tree::Draw_BSP_Tree(Point eye, int type)
{
	double result = Utility::Classify_Point(partition, eye);
	if (result > 0)
	{
		if (back != NULL)
			back->Draw_BSP_Tree(eye, type);
		Draw_Polygon_List(type);
		if (front != NULL)
			front->Draw_BSP_Tree(eye, type);
	}
	else if (result < 0)
	{
		if (front != NULL)
			front->Draw_BSP_Tree(eye, type);
		Draw_Polygon_List(type);
		if (back != NULL)
			back->Draw_BSP_Tree(eye, type);
	}
	else
	{
		if (front != NULL)
			front->Draw_BSP_Tree(eye, type);
		if (back != NULL)
			back->Draw_BSP_Tree(eye, type);
	}
}

void BSP_tree::Add_Polygon(const Polygon& polygon)
{
	if (is_leaf())
	{
		polygons.push_back(polygon);
		partition = Plane(polygon.normal(), -polygon[0].dot(polygon.normal()));
		front = new BSP_tree(true);
		back = new BSP_tree(false);
	}
	else
	{
		int result = Utility::Classify_Polygon(partition, polygon);
		Polygon *p_front = NULL, *p_back = NULL;
		switch (result)
		{
		case COINCIDENT:
			polygons.push_back(polygon);
			break;
		case IN_BACK_OF:
			p_back = new Polygon(polygon);
			break;
		case IN_FRONT_OF:
			p_front = new Polygon(polygon);
			break;
		case SPAN:
			//Polygon *front_piece, *back_piece;
			polygon.Split_Polygon(partition, p_front, p_back);
			
			break;
		default:
			break;
		}
		
		//polygon.Split_Polygon(partition, p_front, p_back);
		//if (!p_front->Size() && !p_back->Size())
			//back->Add_Polygon(polygon);
		//else
		//{
			if (p_front && p_front->Size())
				front->Add_Polygon(*p_front);
			if (p_back && p_back->Size())
				back->Add_Polygon(*p_back);
		//}
	}
}

void BSP_tree::Shadow(const Point& light, const Polygon& polygon, vector<Polygon>*& lit_polygons, vector<Polygon>*& shadow_polygons)
{
	if (is_leaf())
	{
		if (is_out)
		{
			int size = polygon.Size();
			for (int i = 0; i < size; i++)
			{
				Add_Polygon(Polygon(light, polygon[i], polygon[(i + 1) % size]));
			}
			lit_polygons->push_back(polygon);
		}
		else
			shadow_polygons->push_back(polygon);
	}
	else
	{
		int result = Utility::Classify_Polygon(partition, polygon);
		Polygon *p_front = NULL, *p_back = NULL;
		switch (result)
		{
		case COINCIDENT:
			polygons.push_back(polygon);
			break;
		case IN_BACK_OF:
			p_back = new Polygon(polygon);
			break;
		case IN_FRONT_OF:
			p_front = new Polygon(polygon);
			break;
		case SPAN:
			polygon.Split_Polygon(partition, p_front, p_back);

			break;
		default:
			break;
		}
		//polygon.Split_Polygon(partition, p_front, p_back);
		if (p_front && p_front->Size() )
			front->Shadow(light, *p_front, lit_polygons, shadow_polygons);
		if (p_back && p_back->Size())
			back->Shadow(light, *p_back, lit_polygons, shadow_polygons);
	}
	
}

void BSP_tree::Draw_Shadow(BSP_tree*& ShadowBSP, const Point& light, std::vector<Polygon>*& lit_polygons, std::vector<Polygon>*& shadow_polygons)
{
	double result = Utility::Classify_Point(partition, light);
	if (result < 0)
	{
		if (back != NULL)
			back->Draw_Shadow(ShadowBSP, light, lit_polygons, shadow_polygons);

		vector<Polygon>::iterator iter = polygons.begin();
		while (iter != polygons.end())
		{
			if ((*iter).normal().dot(light - (*iter)[0]) >= 0)
				ShadowBSP->Shadow(light, *iter, lit_polygons, shadow_polygons);
			else
				shadow_polygons->push_back(*iter);
			iter++;
		}

		if (front != NULL)
			front->Draw_Shadow(ShadowBSP, light, lit_polygons, shadow_polygons);
	}
	else if (result > 0)
	{
		if (front != NULL)
			front->Draw_Shadow(ShadowBSP, light, lit_polygons, shadow_polygons);

		vector<Polygon>::iterator iter = polygons.begin();
		while (iter != polygons.end())
		{
			if ((*iter).normal().dot(light - (*iter)[0]) >= 0)
				ShadowBSP->Shadow(light, *iter, lit_polygons, shadow_polygons);
			else
				shadow_polygons->push_back(*iter);
			iter++;
		}

		if (back != NULL)
			back->Draw_Shadow(ShadowBSP, light, lit_polygons, shadow_polygons);
	}
	else
	{
		if (front != NULL)
			front->Draw_Shadow(ShadowBSP, light, lit_polygons, shadow_polygons);

		if (back != NULL)
			back->Draw_Shadow(ShadowBSP, light, lit_polygons, shadow_polygons);
	}
}

void BSP_tree::near_to_far(const Point& light, std::vector<Polygon>& sort_list)
{
	double result = Utility::Classify_Point(partition, light);
	if (result < 0)
	{
		if (back != NULL)
			back->near_to_far(light, sort_list);

		for (int i = 0; i < polygons.size(); i++)
			if (polygons[i].Size() > 2)
				sort_list.push_back(Polygon(polygons[i]));


		if (front != NULL)
			front->near_to_far(light, sort_list);
	}
	else if (result > 0)
	{
		if (front != NULL)
			front->near_to_far(light, sort_list);

		for (int i = 0; i < polygons.size(); i++)
			if(polygons[i].Size() > 2)
				sort_list.push_back(Polygon(polygons[i]));

		if (back != NULL)
			back->near_to_far(light, sort_list);
	}
	else
	{
		if (front != NULL)
			front->near_to_far(light, sort_list);

		if (back != NULL)
			back->near_to_far(light, sort_list);
	}
}