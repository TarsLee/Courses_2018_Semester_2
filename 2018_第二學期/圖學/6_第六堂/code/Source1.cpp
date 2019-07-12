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
#include <math.h>

using namespace std;

void instructions();

void grid_size_Menu(int id);
void mainMenu(int id);

void init();
void SetupRC();
void RenderScence();
void Mouse(int button, int state, int x, int y);
void ChangeWindowShape(int w, int h);
void mouse_and_grid_points();

/*	for mouse	*/
float mouse_X = 0;
float mouse_Y = 0;

// 預設格數為 3格
float input_number = 3;
float mouse_offset_x;
float mouse_offset_y;

int window_width = 500;
int window_height = 500;

// Draw grid or not
bool draw_grid = 0;

int main(int argc, char** argv)
{
	int grid_size_menu;
	
	instructions();

	/* Set graphics window parameters. */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("obj_file_Parser");

	init();
	SetupRC();

	grid_size_menu = glutCreateMenu(grid_size_Menu);
	glutAddMenuEntry("5 x 5", 1);
	glutAddMenuEntry("25 x 25", 2);
	glutAddMenuEntry("85 x 85", 3);


	glutCreateMenu(mainMenu);

	glutAddSubMenu("Grid size", grid_size_menu);

	glutAddMenuEntry("Type grid size", 1);
	glutAddMenuEntry("Exit", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	

	// 1. 點擊滑鼠
	glutMouseFunc(Mouse);

	// 2. 渲染模型
	/* Register all callback functions. */
	glutDisplayFunc(RenderScence);
	
	// 3. 視窗與viewport的對應設定
	// Reshape動作與其他動作平行，沒有順序問題
	glutReshapeFunc(ChangeWindowShape);

	/* Initialize the graphics and enter the event loop. */
	glutMainLoop();
}

void instructions()
/* This function displays the instructions to the user. */
{
	cout << "This is a program let mouse to assign a specific grid to fill." << endl;
}

void mainMenu(int id)
/* This is the callback function for the main menu. */
{
	double lineWidth, color[4];

	switch (id)
	{
	case 1:
		cout << "Type grid size (n * n) : ";
		cin >> input_number;
		system("CLS");
		cout << "Change Grid Size : " << input_number << endl << endl;

		draw_grid = 0;

		glutPostRedisplay();	// redisplayed
		break;
	case 2:
		exit(0);
		break;
	default: /* in case none of the above occur */
		break;
	}
}

void grid_size_Menu(int id)
{
	switch (id)
	{
	case 1:
		input_number = 5;
		draw_grid = 0;
		glutPostRedisplay();	// redisplayed
		break;
	case 2:
		input_number = 25;
		draw_grid = 0;
		glutPostRedisplay();	// redisplayed
		break;
	case 3:
		input_number = 85;
		draw_grid = 0;
		glutPostRedisplay();	// redisplayed
		break;
	default: /* for any case not covered above, leave line thickness unchanged */
		break;
	}
}

void Mouse(int button, int state, int x, int y)
{
	mouse_offset_x = (window_width / 2) / input_number;
	mouse_offset_y = (window_height / 2) / input_number;

	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {

			draw_grid = 1;

			mouse_X = float(x - (window_width / 2)) / mouse_offset_x;
			mouse_Y = float((window_height / 2) - y) / mouse_offset_y;

			//printf("x: %f, y: %f\n", mouse_X, mouse_Y);
			
			mouse_and_grid_points();
		}
	}
}

// grid 的四個邊界點
int x_min;
int y_min;
int x_max;
int y_max;

void mouse_and_grid_points() {
	// 計算 x,y 上下限
	/*cout << "x-min : " << floor(mouse_X) << ",	" << "y-min : " << floor(mouse_Y) << endl;
	cout << "x-max : " << ceil(mouse_X) << ",	" << "y-max : " << ceil(mouse_Y) << endl;*/

	x_min = floor(mouse_X);
	y_min = floor(mouse_Y);
	x_max = ceil(mouse_X);
	y_max = ceil(mouse_Y);

	// 找 4點邊界
	if (x_min % 2 == 0) {
		x_min -= 1;
	}
	if (y_min % 2 == 0) {
		y_min -= 1;
	}
	if (x_max % 2 == 0) {
		x_max += 1;
	}
	if (y_max % 2 == 0) {
		y_max += 1;
	}

	// 計算自定義 Grid座標
	cout << "Grid座標 : (" << (x_max + x_min) / 4 << "," << (y_max + y_min) / 4 << ")" << endl;
}

/*	RenderScence	*/
void RenderScence(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// 調整 攝影機高度
	gluLookAt(0, 0, input_number, 0, 0, 0, 0, 1, 0);
	
	// fill the specific grid
	if (draw_grid == 1) {
		glBegin(GL_POLYGON);
		glColor3f(0.27, 0.7, 0.9);
		glVertex2f(x_max, y_max);
		glVertex2f(x_max, y_min);
		glVertex2f(x_min, y_min);
		glVertex2f(x_min, y_max);
		glEnd();
	}
	
	
	// 一個gird長或寬在座標世界佔2個單位
	int offset = (input_number * 2) / input_number;

	int x_counter = 0;
	int y_counter = 0;

	// 畫網格
	for (int a = -input_number; a <= input_number; a++) { // y-axis
		x_counter = 0;
		for (int s = -input_number; s <= input_number; s++) { // x-axis
			if (x_counter % offset == 0
				&& y_counter % offset == 0) {
				glBegin(GL_LINES);
					glColor3f(1.0, 0.0, 0.0);
					glVertex2i(s, a);
					glVertex2i(s, input_number);
				glEnd();

				glBegin(GL_LINES);
					glColor3f(1.0, 0.0, 0.0);
					glVertex2i(s, a);
					glVertex2i(input_number, a);
				glEnd();
			}
			x_counter++;
		}
		y_counter++;
	}

	glFlush();
	glutSwapBuffers();
}
/*	RenderScence	*/

void ChangeWindowShape(int w, int h) {
	window_width = w;
	window_height = h;

	glViewport(0, 0, window_width, window_height);
	cout << "window_width: " << window_width << ", window_height: " << window_height << endl;
}

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
