#include <corecrt_math_defines.h>
#include "BSP_tree.h"
#include "Polygon.h"
#include "Utility.h"

using namespace std;

#define WIDTH 800
#define HEIGHT 600

#define XSENSITIVITY 0.001
#define YSENSITIVITY 0.001

Point light = Point(0.1, 0.1, 15);

static double dist = 70, direction, pitch = M_PI / 4; // 'Eye' location
static double target[3] = { 0, 0, 0 }; // Target location
static double position[3] = { 100, -200, 100 };
static int lastx, lasty; // Last x and y for dragging the mouse
static int buttonPressed; // The current mouse button 

BSP_tree bsp;
BSP_tree bsp_lit;
BSP_tree bsp_shadow;

// Parses an OBJ file (throwing away most of the gory details
bool ParseOBJ(vector<Polygon>& polygons, const char* objFile, float scale, float tx, float ty, float tz)
{
	double x, y, z;
	vector<Point> points;
	vector<Point> polypoints;
	char buffer[256];
	char* ptr, *end;
	int index;
	
	FILE* file;
	fopen_s(&file, objFile, "r");
	if (!file)
	{
		fprintf(stderr, "Failed to open OBJ file '%s'\n", objFile);
		return false;
	}
	// Read a line
	while (fgets(buffer, 256, file))
	{
		switch (*buffer)
		{
		case 'v': // Vertex definition
			if (buffer[1] != ' ') continue; // ignore normals and texcoords
			x = strtod(buffer + 1, &ptr);
			y = strtod(ptr, &ptr);
			z = strtod(ptr, &ptr);

			printf("v %f %f %f\n", x, y, z);
			points.push_back(Point(x*scale + tx, y*scale + ty, z*scale + tz));
			break;
		case 'f': // Face definition
			polypoints.clear();
			ptr = buffer + 1;
			printf("f ");
			while (true)
			{
				index = strtoul(ptr, &end, 0);
				if (!index) break; // If nothing was parsed, break
				printf("%d ", index);
				ptr = end;
				while (*ptr && !isspace(*ptr)) ++ptr; // Skip to next vertex, ignoring texture and normals
				polypoints.push_back(points[index - 1]);
			}
			printf("\n");
			polygons.push_back(Polygon(&polypoints[0], polypoints.size()));
			break;
		}
	}
	return true;
}


void init()
{
	Plane s(Vec3(1, 1, 1), -1);
	Polygon p1(Point(0, -20, -2), Point(30, 30, -2), Point(-10, 10, -2));
	Polygon p2(Point(0, 0, 0), Point(1, 1, 0), Point(0, 1, 0));

	Polygon p3(Point(0, 0, -1), Point(1, 1, -1), Point(0, 1, -1));

	cout << Utility::Classify_Point(s, Point(2, 2, 2)) << endl;
	cout << Utility::Classify_Polygon(s, p1) << endl;

	Polygon *front_piece, *back_piece;
	p1.Split_Polygon(s, front_piece, back_piece);

	vector<Polygon> list, list1, list2;
	if (ParseOBJ(list, "cube.obj", 5, 1, 1, 5));
		
	if (ParseOBJ(list1, "cube.obj", 5, -1, -1, -5));
	if (ParseOBJ(list2, "cube.obj", 7, 3, 3, -12))

		//list.push_back(p3);
		//list.push_back(p2);
		//list1.push_back(p1);
	list.insert(list.end(), list1.begin(), list1.end());
	list.insert(list.end(), list2.begin(), list2.end());
	bsp.Build_BSP_Tree(list);
	
	
	BSP_tree* shadowBSP = new BSP_tree(true);
	vector<Polygon> *lit = new vector<Polygon>;
	vector<Polygon> *shadow = new vector<Polygon>;
		vector<Polygon> sort_list;
	

	bsp.Draw_Shadow(shadowBSP, light, lit, shadow);
	bsp_lit.Build_BSP_Tree(*lit);
	bsp_shadow.Build_BSP_Tree(*shadow);
	//system("pause");
}

void display()
{
	position[0] = sin(direction)*cos(pitch)*dist + target[0];
	position[1] = sin(pitch)*dist + target[1];
	position[2] = cos(direction)*cos(pitch)*dist + target[2];

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(position[0], position[1], position[2], target[0], target[1], target[2], 0, 0, 1);
	float lp[4] = { 0, 0, 0, 1 };
	float lc[4] = { 1, 1, 1, 1 };
	lp[0] = light.x;
	lp[1] = light.y;
	lp[2] = light.z;
	lc[0] = 1;
	lc[1] = 1;
	lc[2] = 1;
	glLightfv(GL_LIGHT0, GL_POSITION, lp);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lc);
	//glEnable(GL_LIGHTING);
	bsp_lit.Draw_BSP_Tree(Point(position[0], position[1], position[2]), 1);
	bsp_shadow.Draw_BSP_Tree(Point(position[0], position[1], position[2]), 0);
	glutSwapBuffers();
}

void Mouse(int button, int state, int x, int y)
{
	buttonPressed = button; // Record current button
	lastx = x;
	lasty = y;
}

void Motion(int x, int y)
{
	if (buttonPressed == 0) // We're dragging
	{
		direction -= (x - lastx)*XSENSITIVITY; // Update direction
		while (direction > 2 * M_PI) // Wrap direction
			direction -= 2 * M_PI;
		while (direction < 0)
			direction += 2 * M_PI;
		pitch += (y - lasty)*YSENSITIVITY; // Update pitch
		if (pitch > M_PI / 2) // Clamp pitch
			pitch = M_PI / 2;
		if (pitch < -M_PI / 2)
			pitch = -M_PI / 2;
	}
	lastx = x; // Update mouse position
	lasty = y;
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("BSP Shadow");

	glutDisplayFunc(display);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);

	glClearColor(0, 0, 0, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, WIDTH / HEIGHT, 0.001, 1000);
	glEnable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL);
	glPointSize(5);

	init();

	glutMainLoop();
	return 0;
}