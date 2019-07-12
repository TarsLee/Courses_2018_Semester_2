/**************************************************************
*                                                            *
* This program demonstrates menus in OpenGL and GLUT. It     *
* uses a menu to change the drawing color and the line       *
* thickness. The drawing area can also be cleared and the    *
* program exited from the menu.                              *
*                                                            *
* Author:  Cary Laxer                                        *
*                                                            *
* Modification History:                                      *
*    September 18, 1997 - Creation.                          *
*    September 16, 1998 - Added an instructions function for *
*       displaying instructions to the user.                 *
*    September 11, 2000 - Converted C I/O to C++ I/O.        *
*                                                            *
**************************************************************/

#include <iostream>
#include <stdlib.h>
#include <GL/glut.h>
#include <time.h>
#include <fstream>
#include <string>

#define SIZE 300

void instructions();
void gfxinit();
void display(void);
//void colorMenu(int id);
//void sizeMenu(int id);

void render_mode_Menu(int id);
void file_Menu(int id);
void color_mode_Menu(int id);
void bounding_box_Menu(int id);
void axis_Menu(int id);

void mainMenu(int id);

int pointsChosen, x1, y1_y, numberOfLists = 0;
GLsizei ysize;

using namespace std;

void init();
void SetupRC();
void RenderScence();
void readFile(const char *path);
void draw_point_func();
void init_resource();
void NormalKeyHandler(unsigned char key, int x, int y);
void AllReset();
void MatrixReset();

float* Faces_Triangles;					// Stores the triangles
float* vertexBuffer;					// Stores the points which make the object
long TotalConnectedPoints;				// Stores the total number of connected verteces
long TotalConnectedTriangles;			// Stores the total number of connected triangles

#define POINTS_PER_VERTEX 3
#define TOTAL_FLOATS_IN_TRIANGLE 9

const char *file_path = "./obj_file/gourd.obj";

float x;
float y;
float z;

bool bounding_box_on = 0;
bool axis_on = 1;
float r = 0.0;
float g = 0.0;
float b = 0.0;

/*	for normal keyboard	*/
GLfloat gl_tran_x = 0;
GLfloat gl_tran_y = 0;
GLfloat gl_tran_z = 0;

GLfloat gl_rot_x = 0;
GLfloat gl_rot_y = 0;
GLfloat gl_rot_z = 0;

#define M_PI 3.14159265358979323846

int main(int argc, char** argv)
{
	/*	0406	*/
	srand(time(NULL));
	/*	0406	*/

	//int color_menu, size_menu;
	// define by Tars
	int file_menu;
	int render_mode_menu;
	int color_mode_menu;
	int bounding_box_menu;
	int axis_menu;

	instructions();

	/* Set graphics window parameters. */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("obj_file_Parser");

	init();
	SetupRC();


	/* Create the menu structure and attach it to the right mouse button. */

	/*
	color_menu = glutCreateMenu(colorMenu);
	glutAddMenuEntry("Red", 1);
	glutAddMenuEntry("Green", 2);
	glutAddMenuEntry("Blue", 3);
	glutAddMenuEntry("Black", 4);
	size_menu = glutCreateMenu(sizeMenu);
	glutAddMenuEntry("1", 1);
	glutAddMenuEntry("2", 2);
	glutAddMenuEntry("3", 3);
	*/

	// define by Tars
	file_menu = glutCreateMenu(file_Menu);
	glutAddMenuEntry("gourd", 1);
	glutAddMenuEntry("lamp", 2);
	glutAddMenuEntry("octahedron", 3);
	glutAddMenuEntry("teapot", 4);
	glutAddMenuEntry("teddy", 5);
	glutAddMenuEntry("others...", 6);
	render_mode_menu = glutCreateMenu(render_mode_Menu);
	glutAddMenuEntry("Point", 1);
	glutAddMenuEntry("Line", 2);
	glutAddMenuEntry("Face", 3);
	color_mode_menu = glutCreateMenu(color_mode_Menu);
	glutAddMenuEntry("Black", 1);
	glutAddMenuEntry("Red", 2);
	glutAddMenuEntry("Green", 3);
	glutAddMenuEntry("Blue", 4);
	glutAddMenuEntry("Random color", 5);
	bounding_box_menu = glutCreateMenu(bounding_box_Menu);
	glutAddMenuEntry("on", 1);
	glutAddMenuEntry("off", 2);
	axis_menu = glutCreateMenu(axis_Menu);
	glutAddMenuEntry("on", 1);
	glutAddMenuEntry("off", 2);


	glutCreateMenu(mainMenu);
	//glutAddSubMenu("Color", color_menu);
	//glutAddSubMenu("Size", size_menu);

	glutAddSubMenu("File", file_menu);
	glutAddSubMenu("Render Mode", render_mode_menu);
	glutAddSubMenu("Color Mode", color_mode_menu);
	glutAddSubMenu("Bounding Box", bounding_box_menu);
	glutAddSubMenu("Axis", axis_menu);

	glutAddMenuEntry("Exit", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutKeyboardFunc(NormalKeyHandler);

	/* Register all callback functions. */
	glutDisplayFunc(RenderScence);

	/* Initialize the graphics and enter the event loop. */
	glutMainLoop();
}

void instructions()
/* This function displays the instructions to the user. */
{
	cout << "This is a program to parser Obj file." << endl;
}

void mainMenu(int id)
/* This is the callback function for the main menu. */
{
	double lineWidth, color[4];

	switch (id)
	{
	case 1: /* exit the program */
		exit(0);
		break;
	default: /* in case none of the above occur */
		break;
	}
}


//void colorMenu(int id)
///* This is the callback function for the color menu. */
//{
//	glNewList(++numberOfLists, GL_COMPILE_AND_EXECUTE);
//	switch (id)
//	{
//	case 1: /* change color to red */
//		glColor3d(1.0, 0.0, 0.0);
//		break;
//	case 2: /* change color to green */
//		glColor3d(0.0, 1.0, 0.0);
//		break;
//	case 3: /* change color to blue */
//		glColor3d(0.0, 0.0, 1.0);
//		break;
//	case 4: /* change color to black */
//		glColor3d(0.0, 0.0, 0.0);
//		break;
//	default: /* for any case not covered above, leave color unchanged */
//		break;
//	}
//	glEndList();
//	pointsChosen = 0;
//}
//
//void sizeMenu(int id)
///* This is the callback function for the size menu. */
//{
//	glNewList(++numberOfLists, GL_COMPILE_AND_EXECUTE);
//	switch (id)
//	{
//	case 1: /* change line thickness to 1 */
//		glLineWidth(1.0);
//		break;
//	case 2: /* change line thickness to 2 */
//		glLineWidth(2.0);
//		break;
//	case 3: /* change line thickness to 3 */
//		glLineWidth(3.0);
//		break;
//	default: /* for any case not covered above, leave line thickness unchanged */
//		break;
//	}
//	glEndList();
//}

char asd[100] = "";

/*	define by Tars	*/
void file_Menu(int id)
{
	switch (id)
	{
	case 1:
		glDeleteLists(1, numberOfLists);
		numberOfLists = 0;
		init_resource();
		glNewList(++numberOfLists, GL_COMPILE_AND_EXECUTE);
		file_path = "./obj_file/gourd.obj";
		readFile(file_path);
		draw_point_func();
		glEndList();
		break;
	case 2:
		glDeleteLists(1, numberOfLists);
		numberOfLists = 0;
		init_resource();
		glNewList(++numberOfLists, GL_COMPILE_AND_EXECUTE);
		file_path = "./obj_file/lamp.obj";
		readFile(file_path);
		draw_point_func();
		glEndList();
		break;
	case 3:
		glDeleteLists(1, numberOfLists);
		numberOfLists = 0;
		init_resource();
		glNewList(++numberOfLists, GL_COMPILE_AND_EXECUTE);
		file_path = "./obj_file/octahedron.obj";
		readFile(file_path);
		draw_point_func();
		glEndList();
		break;
	case 4:
		glDeleteLists(1, numberOfLists);
		numberOfLists = 0;
		init_resource();
		glNewList(++numberOfLists, GL_COMPILE_AND_EXECUTE);
		file_path = "./obj_file/teapot.obj";
		readFile(file_path);
		draw_point_func();
		glEndList();
		break;
	case 5:
		glDeleteLists(1, numberOfLists);
		numberOfLists = 0;
		init_resource();
		glNewList(++numberOfLists, GL_COMPILE_AND_EXECUTE);
		file_path = "./obj_file/teddy.obj";
		readFile(file_path);
		draw_point_func();
		glEndList();
		break;
	case 6:
		cout << "請輸入檔案路徑 :　";
		cin >> asd;
		file_path = asd;

		glDeleteLists(1, numberOfLists);
		numberOfLists = 0;
		init_resource();
		glNewList(++numberOfLists, GL_COMPILE_AND_EXECUTE);
		readFile(file_path);
		draw_point_func();
		glEndList();
		break;
	default: /* for any case not covered above, leave line thickness unchanged */
		break;
	}
	glutPostRedisplay();	// redisplayed
	cout << file_path << endl;
}

void render_mode_Menu(int id)
{
	// 先讀檔，計算每一組點,線,面
	readFile(file_path);


	switch (id)
	{
	case 1:
		glDeleteLists(1, numberOfLists);
		numberOfLists = 0;
		init_resource();
		draw_point_func();
		break;
	case 2:
		glDeleteLists(1, numberOfLists);
		numberOfLists = 0;
		init_resource();
		glNewList(++numberOfLists, GL_COMPILE_AND_EXECUTE);
		/*	Draw Line of the face	*/
		for (int i = 0; i < (TotalConnectedTriangles / 9); i++) {
			glBegin(GL_LINES);

			//glColor3f(0.0, 1.0, 0.0);
			/*
			int j;
			glClear(GL_COLOR_BUFFER_BIT);
			for (j = 1; j <= numberOfLists; j++) glCallList(j);
			*/

			glVertex3f(Faces_Triangles[i * 9], Faces_Triangles[i * 9 + 1], Faces_Triangles[i * 9 + 2]);  // Vertex 1
			glVertex3f(Faces_Triangles[i * 9 + 3], Faces_Triangles[i * 9 + 4], Faces_Triangles[i * 9 + 5]);  // Vertex 2
			glVertex3f(Faces_Triangles[i * 9 + 3], Faces_Triangles[i * 9 + 4], Faces_Triangles[i * 9 + 5]);  // Vertex 2
			glVertex3f(Faces_Triangles[i * 9 + 6], Faces_Triangles[i * 9 + 7], Faces_Triangles[i * 9 + 8]);  // Vertex 3
			glVertex3f(Faces_Triangles[i * 9], Faces_Triangles[i * 9 + 1], Faces_Triangles[i * 9 + 2]);  // Vertex 1
			glVertex3f(Faces_Triangles[i * 9 + 6], Faces_Triangles[i * 9 + 7], Faces_Triangles[i * 9 + 8]);  // Vertex 3
			glEnd();

			/*printf("9個點\n");
			printf("%f %f %f\n", Faces_Triangles[i * 9], Faces_Triangles[i * 9 + 1], Faces_Triangles[i * 9 + 2]);
			printf("%f %f %f\n", Faces_Triangles[i * 9 + 3], Faces_Triangles[i * 9 + 4], Faces_Triangles[i * 9 + 5]);
			printf("%f %f %f\n", Faces_Triangles[i * 9 + 6], Faces_Triangles[i * 9 + 7], Faces_Triangles[i * 9 + 8]);*/
		}
		cout << "組共有 " << TotalConnectedTriangles / 9 << " 個面" << endl;
		/*	Draw Line of the face	*/
		glEndList();
		break;
	case 3:
		glDeleteLists(1, numberOfLists);
		numberOfLists = 0;
		init_resource();
		glNewList(++numberOfLists, GL_COMPILE_AND_EXECUTE);
		/*	Draw the face	*/
		for (int i = 0; i < (TotalConnectedTriangles / 9); i++) {
			glBegin(GL_TRIANGLES);

			//glColor3f(0.0, 0.0, 1.0);
			/*
			int j;
			glClear(GL_COLOR_BUFFER_BIT);
			for (j = 1; j <= numberOfLists; j++) glCallList(j);
			*/

			glVertex3f(Faces_Triangles[i * 9], Faces_Triangles[i * 9 + 1], Faces_Triangles[i * 9 + 2]);  // Vertex 1
			glVertex3f(Faces_Triangles[i * 9 + 3], Faces_Triangles[i * 9 + 4], Faces_Triangles[i * 9 + 5]);  // Vertex 2
			glVertex3f(Faces_Triangles[i * 9 + 6], Faces_Triangles[i * 9 + 7], Faces_Triangles[i * 9 + 8]);  // Vertex 3
			glEnd();

		}
		/*	Draw the face	*/
		glEndList();
		break;
	case 4:
		glColor3d(0.0, 0.0, 1.0);		// blue color
		break;
	case 5:
		x = (float)rand() / (RAND_MAX + 1.0);
		y = (float)rand() / (RAND_MAX + 1.0);
		z = (float)rand() / (RAND_MAX + 1.0);

		glColor3d(x, y, z);		// random color
		break;
	default: /* for any case not covered above, leave line thickness unchanged */
		break;
	}
	glutPostRedisplay();	// redisplayed
}

void color_mode_Menu(int id)
/* This is the callback function for the color menu. */
{
	glNewList(++numberOfLists, GL_COMPILE_AND_EXECUTE);
	switch (id)
	{
	case 1:
		glColor3d(0.0, 0.0, 0.0);		// black color
		r = 0.0;
		g = 0.0;
		b = 0.0;
		break;
	case 2:
		glColor3d(1.0, 0.0, 0.0);		// red color
		r = 1.0;
		g = 0.0;
		b = 0.0;
		break;
	case 3:
		glColor3d(0.0, 1.0, 0.0);		// green color
		r = 0.0;
		g = 1.0;
		b = 0.0;
		break;
	case 4:
		glColor3d(0.0, 0.0, 1.0);		// blue color
		r = 0.0;
		g = 0.0;
		b = 1.0;
		break;
	case 5:
		x = (float)rand() / (RAND_MAX + 1.0);
		y = (float)rand() / (RAND_MAX + 1.0);
		z = (float)rand() / (RAND_MAX + 1.0);
		glColor3d(x, y, z);		// random color

		r = z;
		g = x;
		b = y;
		break;
	default: /* for any case not covered above, leave line thickness unchanged */
		break;
	}
	glEndList();
	glutPostRedisplay();	// redisplayed
}

void bounding_box_Menu(int id)
{
	
	switch (id)
	{
		case 1:
			bounding_box_on = 1;
			break;
		case 2:
			bounding_box_on = 0;
			break;
		default: /* for any case not covered above, leave line thickness unchanged */
			break;
	}
	glutPostRedisplay();	// redisplayed
}

void axis_Menu(int id)
{
	switch (id)
	{
		case 1:
			axis_on = 1;
			break;
		case 2:
			axis_on = 0;
			break;
		default: /* for any case not covered above, leave line thickness unchanged */
			break;
	}
	glutPostRedisplay();	// redisplayed
}
/*	define by Tars	*/

// 運算矩陣
GLfloat multiMatrix[16];

// initial matrix
void MatrixReset() {
	multiMatrix[0] = 1; multiMatrix[1] = 0; multiMatrix[2] = 0; multiMatrix[3] = 0;
	multiMatrix[4] = 0; multiMatrix[5] = 1; multiMatrix[6] = 0; multiMatrix[7] = 0;
	multiMatrix[8] = 0; multiMatrix[9] = 0; multiMatrix[10] = 1; multiMatrix[11] = 0;
	multiMatrix[12] = 0; multiMatrix[13] = 0; multiMatrix[14] = 0; multiMatrix[15] = 1;
}

void TranslateMatrix(GLfloat X, GLfloat Y, GLfloat Z) {
	MatrixReset();
	multiMatrix[12] = X;
	multiMatrix[13] = Y;
	multiMatrix[14] = Z;
	glMultMatrixf(multiMatrix);
}

void RotateMatrix(float angle, float x, float y, float z) {
	MatrixReset();
	GLfloat Cos = cos(angle * M_PI / 180);
	GLfloat Sin = sin(angle * M_PI / 180);

	// for x,y,z rotate
	multiMatrix[0] = Cos + (1 - Cos) * x * x;
	multiMatrix[1] = (1 - Cos) * x * y + Sin * z;
	multiMatrix[2] = (1 - Cos) * x * z - Sin * y;
	multiMatrix[4] = (1 - Cos) * x * y - Sin * z;
	multiMatrix[5] = Cos + (1 - Cos) * y * y;
	multiMatrix[6] = (1 - Cos) * z * y + Sin * x;
	multiMatrix[8] = (1 - Cos) * x * z + Sin * y;
	multiMatrix[9] = (1 - Cos) * y * z - Sin * x;
	multiMatrix[10] = Cos + (1 - Cos) * z * z;
	glMultMatrixf(multiMatrix);

}

void AllReset() {
	// for x,y,z rotate
	gl_tran_x = 0;
	gl_tran_y = 0;
	gl_tran_z = 0;

	// for x,y,z rotate
	gl_rot_x = 0;
	gl_rot_y = 0;
	gl_rot_z = 0;
}

float ortho = 20.0;

/*	RenderScence	*/
void RenderScence(void)
{
	// 先讀檔，計算每一組點,線,面
	readFile(file_path);
	//cout << "real file : " << file_path << endl;


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, ortho, 0, 0, 0, 0, 1, 0);
		   
	// draw x_axis, y_axis, z_axis
	if (axis_on == 1) {
		glBegin(GL_LINES);
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(500, 0, 0);
		glVertex3f(-500, 0, 0);
		glVertex3f(0, 500, 0);
		glVertex3f(0, -500, 0);
		glVertex3f(0, 0, 500);
		glVertex3f(0, 0, -500);
		glEnd();
		/*	fix color delay bug	*/
		glColor3f(r, g, b);
		//cout << "ON : " << r << g << b << endl;
	} else {
		/*	fix color delay bug	*/
		glColor3f(r, g, b);
		//cout << "OFF : " << r << g << b << endl;
	}

	//旋轉
	RotateMatrix(gl_rot_x, 1.0, 0.0, 0.0);
	RotateMatrix(gl_rot_y, 0.0, 1.0, 0.0);
	RotateMatrix(gl_rot_z, 0.0, 0.0, 1.0);

	//位移
	TranslateMatrix(gl_tran_x, gl_tran_y, gl_tran_z);

	/*	Find min_max point for x,y,z	*/
	float min_x = 0, min_y = 0, min_z = 0;
	float max_x = 0, max_y = 0, max_z = 0;

	for (int i = 0; i < (TotalConnectedPoints / 3); i++) {

		//glColor3f(1.0, 0.0, 0.0);

		if (vertexBuffer[i * 3] < min_x)	min_x = vertexBuffer[i * 3];
		if (vertexBuffer[i * 3 + 1] < min_y)	min_y = vertexBuffer[i * 3 + 1];
		if (vertexBuffer[i * 3 + 2] < min_z)	min_z = vertexBuffer[i * 3 + 2];
		if (vertexBuffer[i * 3] > max_x)	max_x = vertexBuffer[i * 3];
		if (vertexBuffer[i * 3 + 1] > max_y)	max_y = vertexBuffer[i * 3 + 1];
		if (vertexBuffer[i * 3 + 2] > max_z)	max_z = vertexBuffer[i * 3 + 2];

	}

	if ((max_x - min_x) > (max_y - min_y)) {
		ortho = (max_x - min_x);
	} else {
		ortho = (max_y - min_y);
	}
	if (ortho < 4) {
		ortho = 4;
	}

	cout << "Ortho : " << ortho << endl;

	cout << "最小 x : " << min_x << endl;
	cout << "最小 y : " << min_y << endl;
	cout << "最小 z : " << min_z << endl;
	cout << "最大 x : " << max_x << endl;
	cout << "最大 y : " << max_y << endl;
	cout << "最大 z : " << max_z << endl;

	if (bounding_box_on == 1) {

		glBegin(GL_LINES);
		glColor3f(0.0, 1.0, 1.0);
		glVertex3f(min_x, min_y, min_z);
		glVertex3f(min_x, min_y, max_z);
		glVertex3f(min_x, min_y, min_z);
		glVertex3f(max_x, min_y, min_z);
		glVertex3f(min_x, min_y, min_z);
		glVertex3f(min_x, max_y, min_z);
		glVertex3f(max_x, max_y, max_z);
		glVertex3f(max_x, max_y, min_z);
		glVertex3f(max_x, max_y, max_z);
		glVertex3f(max_x, min_y, max_z);
		glVertex3f(max_x, max_y, max_z);
		glVertex3f(min_x, max_y, max_z);
		glVertex3f(max_x, max_y, min_z);
		glVertex3f(max_x, min_y, min_z);
		glVertex3f(max_x, max_y, min_z);
		glVertex3f(min_x, max_y, min_z);
		glVertex3f(min_x, min_y, max_z);
		glVertex3f(min_x, max_y, max_z);
		glVertex3f(min_x, min_y, max_z);
		glVertex3f(max_x, min_y, max_z);
		glVertex3f(max_x, min_y, min_z);
		glVertex3f(max_x, min_y, max_z);
		glVertex3f(min_x, max_y, min_z);
		glVertex3f(min_x, max_y, max_z);
		glEnd();

		glPointSize(5.0);
		glBegin(GL_POINTS);
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(min_x, min_y, min_z);
		glVertex3f(min_x, min_y, max_z);
		glVertex3f(min_x, max_y, min_z);
		glVertex3f(max_x, min_y, min_z);
		glVertex3f(min_x, max_y, max_z);
		glVertex3f(max_x, max_y, max_z);
		glVertex3f(max_x, min_y, max_z);
		glVertex3f(max_x, max_y, min_z);
		glEnd();
		/*	Find min_max point for x,y,z	*/

		/*	fix color delay bug	*/
		glColor3f(r, g, b);
		//cout << "ON : " << r << g << b << endl;
	} else {
		/*	fix color delay bug	*/
		glColor3f(r, g, b);
	}

	int j;
	for (j = 1; j <= numberOfLists; j++) glCallList(j);
	//for (j = numberOfLists; j >= 1; j--) glCallList(j);
	

	// init color for object
	// _1_ is the first number to control table
	
	if (numberOfLists == 1) {
		init_resource();
	}

	glFlush();
	glutSwapBuffers();

	/*	free vertexBuffer and Faces_Triangles	*/
	free(vertexBuffer);
	free(Faces_Triangles);

	vertexBuffer = NULL;
	Faces_Triangles = NULL;
	/*	free vertexBuffer and Faces_Triangles	*/
}
/*	RenderScence	*/

/*	read file	*/
void readFile(const char *path) {

	string line;
	ifstream objFile(path);

	TotalConnectedPoints = 0;
	TotalConnectedTriangles = 0;

	if (objFile.is_open())													// If obj file is open, continue
	{
		objFile.seekg(0, ios::end);										// Go to end of the file, 
		long fileSize = objFile.tellg();									// get file size
		objFile.seekg(0, ios::beg);										// we'll use this to register memory for our 3d model
		//cout << "Size : " << fileSize << endl;

		vertexBuffer = (float*)malloc(fileSize);							// Allocate memory for the verteces
		Faces_Triangles = (float*)malloc(fileSize * sizeof(float));			// Allocate memory for the triangles

		int triangle_index = 0;												// Set triangle index to zero


		while (!objFile.eof())											// Start reading file data
		{
			getline(objFile, line);

			if (line[0] == 'v' && line[1] == ' ') {

				line[0] = ' ';												// Set first character to 0. This will allow us to use sscanf

				sscanf_s(line.c_str(), "%f %f %f ", 						// Read floats from the line: v X Y Z
					&vertexBuffer[TotalConnectedPoints],
					&vertexBuffer[TotalConnectedPoints + 1],
					&vertexBuffer[TotalConnectedPoints + 2]);

				/*cout << "頂點" << " " << vertexBuffer[TotalConnectedPoints]
					<< " " << vertexBuffer[TotalConnectedPoints + 1]
					<< " " << vertexBuffer[TotalConnectedPoints + 2] << endl;*/

				TotalConnectedPoints += POINTS_PER_VERTEX;
			}

			if (line[0] == 'f' && line[1] == ' ') {

				line[0] = ' ';												// Set first character to 0. This will allow us to use sscanf

				int vertexNumber[4] = { 0, 0, 0 };
				sscanf_s(line.c_str(), "%i %i %i",								// Read integers from the line:  f 1 2 3
					&vertexNumber[0],										// First point of our triangle. This is an 
					&vertexNumber[1],										// pointer to our vertexBuffer list
					&vertexNumber[2]);										// each point represents an X,Y,Z.

				vertexNumber[0] -= 1;										// OBJ file starts counting from 1
				vertexNumber[1] -= 1;										// OBJ file starts counting from 1
				vertexNumber[2] -= 1;										// OBJ file starts counting from 1

				int tCounter = 0;
				for (int i = 0; i < POINTS_PER_VERTEX; i++)
				{
					Faces_Triangles[triangle_index + tCounter] = vertexBuffer[3 * vertexNumber[i]];
					Faces_Triangles[triangle_index + tCounter + 1] = vertexBuffer[3 * vertexNumber[i] + 1];
					Faces_Triangles[triangle_index + tCounter + 2] = vertexBuffer[3 * vertexNumber[i] + 2];

					tCounter += POINTS_PER_VERTEX;
				}

				// 記錄是第幾個面
				triangle_index += TOTAL_FLOATS_IN_TRIANGLE;
				// 記錄總共有幾個面
				TotalConnectedTriangles += TOTAL_FLOATS_IN_TRIANGLE;
				//cout << "面" << " " << x << " " << y << " " << z << endl;
			}
		}
	}
	// 讀檔時
	cout << "讀檔時..." << endl;
	cout << "共有幾個頂點: " << TotalConnectedPoints / 3 << endl;
	cout << "組共有 " << TotalConnectedTriangles / 9 << " 個面" << endl;
}
/*	read file	*/

void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glFrustum(-1, 1, -1, 1, 1, 1000);
	glEnable(GL_DEPTH_TEST);
}

void SetupRC()
{
	// Light values and coordinates
	GLfloat whiteLight[] = { 0.45f, 0.45f, 0.45f, 1.0f };
	GLfloat sourceLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
	GLfloat lightPos[] = { 0.f, 25.0f, 20.0f, 0.0f };
	// Enable lighting
	glEnable(GL_LIGHTING);
	// Setup and enable light 0
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, sourceLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sourceLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);
	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
}

void draw_point_func() {
	glNewList(++numberOfLists, GL_COMPILE_AND_EXECUTE);
	/*	Draw Point of the Object file	*/
	for (int i = 0; i < (TotalConnectedPoints / 3); i++) {
		glPointSize(5.0);

		//glColor3f(1.0, 0.0, 0.0);
		/*
		int j;
		glClear(GL_COLOR_BUFFER_BIT);
		for (j = 1; j <= numberOfLists; j++) glCallList(j);
		*/

		glBegin(GL_POINTS);
		glVertex3f(vertexBuffer[i * 3], vertexBuffer[i * 3 + 1], vertexBuffer[i * 3 + 2]);
		glEnd();
	}
	cout << "共有幾個頂點: " << TotalConnectedPoints / 3 << endl;
	/*	Draw Point of the Object file	*/
	glEndList();
}

// after function must attach "glutPostRedisplay();" for redisplayed
// "draw_point_func();" function is optional
void init_resource() {
	glColor3f(1.0, 0.0, 0.0);
	r = 1.0;
	g = 0.0;
	b = 0.0;
}

void NormalKeyHandler(unsigned char key, int x, int y)
{
	switch (key)
	{

		// transform
		// x-axis
		case 'i':
			gl_tran_y += 1.0f;
			break;
		case 'k':
			gl_tran_y -= 1.0f;
			break;
			// y-axis
		case 'j':
			gl_tran_x -= 1.0f;
			break;
		case 'l':
			gl_tran_x += 1.0f;
			break;
			// z-axis
		case 'o':
			gl_tran_z += 1.0f;
			break;
		case 'u':
			gl_tran_z -= 1.0f;
			break;

		// rotation
		// x-axis
		case 'q':
			gl_rot_y += 1.0f;
			break;
		case 'a':
			gl_rot_y -= 1.0f;
			break;
			// y-axis
		case 'w':
			gl_rot_x += 1.0f;
			break;
		case 's':
			gl_rot_x -= 1.0f;
			break;
			// z-axis
		case 'e':
			gl_rot_z += 1.0f;
			break;
		case 'd':
			gl_rot_z -= 1.0f;
			break;

		//reset
		case 'r':
			AllReset();
			break;
	}
	//刷新窗口
	glutPostRedisplay();
}