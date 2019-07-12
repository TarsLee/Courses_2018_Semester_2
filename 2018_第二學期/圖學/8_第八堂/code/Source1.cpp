#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <time.h>
#include <fstream>
#include <string>
#include <math.h>

using namespace std;

void instructions();

void grid_size_Menu(int id);
void line_algo_Menu(int id);
void mainMenu(int id);

void init();
void SetupRC();
void RenderScence();
void Mouse(int button, int state, int x, int y);
void ChangeWindowShape(int w, int h);
void mouse_and_grid_points();
void init_grid_canvas();

int round_number(float value);
void anti_aliased_algo(int x1, int y1, int x2, int y2);
void draw_grid_func(int *grid_info, bool x_trade, bool y_trade, float alpha);
void midpoint_algo_compute_grid(int x1, int y1, int x2, int y2);
void grid_array_2_process();
void fill_triangle(int *x_array_2, int *y_array_2, int array_count_2, int min_y_grid, int max_y_grid);
void draw_inside_grid(int *x_array, int *y_array, int min_x, int max_x, int min_y, int max_y, int grid_x, int grid_y);
void writePixel_3(int pixel_x, int pixel_y);

void draw_mid_antialiased_part1(float x, float x2, float y, float y2, float dx, float dy, float m);
void draw_mid_antialiased_part2(float x, float x2, float y, float y2, float dx, float dy);
void draw_mid_antialiased_part3(float x, float x2, float y, float y2, float dx, float dy);
void draw_mid_antialiased_part4(float x, float x2, float y, float y2, float dx, float dy);

/*	for mouse	*/
float mouse_X = 0;
float mouse_Y = 0;

// 預設格數為 3格
float input_number = 3;
float mouse_offset_x;
float mouse_offset_y;

int window_width = 500;
int window_height = 500;

// 有點擊第二次，有end_point
int have_second_point = 0;

// Draw grid or not
bool draw_grid = 0;

bool midpoint_used = 0;
bool anti_aliased_used = 0;
bool midpoint_and_anti_aliased_used = 0;

int main(int argc, char** argv)
{
	int grid_size_menu;
	int line_algo_menu;

	instructions();

	/* Set graphics window parameters. */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Fill a triangle with three grids");

	init();
	SetupRC();

	grid_size_menu = glutCreateMenu(grid_size_Menu);
	glutAddMenuEntry("5 x 5", 1);
	glutAddMenuEntry("25 x 25", 2);
	glutAddMenuEntry("85 x 85", 3);

	line_algo_menu = glutCreateMenu(line_algo_Menu);
	glutAddMenuEntry("Midpoint line algo", 1);
	glutAddMenuEntry("Anti-aliased line algo", 2);
	glutAddMenuEntry("Midpoint + Anti-aliased line algo", 3);

	glutCreateMenu(mainMenu);

	glutAddSubMenu("Grid size", grid_size_menu);
	glutAddSubMenu("Line algorithm", line_algo_menu);

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

		init_grid_canvas();

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
		init_grid_canvas();
		glutPostRedisplay();	// redisplayed
		break;
	case 2:
		input_number = 25;
		init_grid_canvas();
		glutPostRedisplay();	// redisplayed
		break;
	case 3:
		input_number = 85;
		init_grid_canvas();
		glutPostRedisplay();	// redisplayed
		break;
	default: /* for any case not covered above, leave line thickness unchanged */
		break;
	}
}

void line_algo_Menu(int id)
{
	switch (id)
	{
	case 1:
		midpoint_used = 1;
		anti_aliased_used = 0;
		midpoint_and_anti_aliased_used = 0;
		break;
	case 2:
		midpoint_used = 0;
		anti_aliased_used = 1;
		midpoint_and_anti_aliased_used = 0;
		break;
	case 3:
		midpoint_used = 1;
		anti_aliased_used = 0;
		midpoint_and_anti_aliased_used = 1;
		break;
	default: /* for any case not covered above, leave line thickness unchanged */
		break;
	}
}

// grid 的四個邊界點
int x_min;
int y_min;
int x_max;
int y_max;

int x_min_2;
int y_min_2;
int x_max_2;
int y_max_2;

int grid_pos_x;
int grid_pos_y;

int grid_pos_x_2;
int grid_pos_y_2;

void Mouse(int button, int state, int x, int y)
{
	mouse_offset_x = (window_width / 2) / input_number;
	mouse_offset_y = (window_height / 2) / input_number;

	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {

			draw_grid = 1;

			// have second point
			have_second_point++;

			if (have_second_point >= 2) {
				x_min_2 = x_min;
				y_min_2 = y_min;
				x_max_2 = x_max;
				y_max_2 = y_max;

				grid_pos_x_2 = grid_pos_x;
				grid_pos_y_2 = grid_pos_y;
			}

			// 真正滑鼠點的 x和y
			mouse_X = float(x - (window_width / 2)) / mouse_offset_x;
			mouse_Y = float((window_height / 2) - y) / mouse_offset_y;

			mouse_and_grid_points();
		}
	}
}


int grid_pos_x_array[3];
int grid_pos_y_array[3];
int grid_pos_array_count = 0;
bool three_click_up = 0;

void mouse_and_grid_points() {
	// 計算 x,y 上下限
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

	// 網格的 x,y位置
	grid_pos_x = (x_max + x_min) / 4;
	grid_pos_y = (y_max + y_min) / 4;

	grid_pos_x_array[grid_pos_array_count] = grid_pos_x;
	grid_pos_y_array[grid_pos_array_count] = grid_pos_y;
	grid_pos_array_count++;
	
	if(grid_pos_array_count > 2){
		grid_pos_array_count = 0;
		three_click_up = 1;
	}

	// 計算自定義 Grid座標
	cout << " ---------------------------------------- " << endl;

	if (have_second_point >= 2) {
		cout << "Start Grid座標 : (" << grid_pos_x_2 << "," << grid_pos_y_2 << "), ";
		cout << "End Grid座標 : (" << grid_pos_x << "," << grid_pos_y << ")" << endl;
	}
	else {
		cout << "Start Grid座標 : (" << grid_pos_x << "," << grid_pos_x << ")" << endl;
	}
}

// line slope
float m;

int x_grid_array[500];
int y_grid_array[500];
int grid_array_count = 0;

int x_grid_array_2[500];
int y_grid_array_2[500];
int grid_array_count_2 = 0;

int the_min_x_grid = 88;
int the_max_x_grid = -88;
int the_min_y_grid = 88;
int the_max_y_grid = -88;

/*	RenderScence	*/
void RenderScence(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// 調整 攝影機高度
	gluLookAt(0, 0, input_number, 0, 0, 0, 0, 1, 0);
	   
	if (three_click_up) {

		// draw three vertex
		cout << "asd" << endl;
		for (int i = 0; i < 3; i++) {
			cout << grid_pos_x_array[i] << ", " << grid_pos_y_array[i] << endl;
			writePixel_3(grid_pos_x_array[i], grid_pos_y_array[i]);
		}

		// 畫線
		midpoint_algo_compute_grid(grid_pos_x_array[0], grid_pos_y_array[0], grid_pos_x_array[1], grid_pos_y_array[1]);
		midpoint_algo_compute_grid(grid_pos_x_array[1], grid_pos_y_array[1], grid_pos_x_array[2], grid_pos_y_array[2]);
		midpoint_algo_compute_grid(grid_pos_x_array[2], grid_pos_y_array[2], grid_pos_x_array[0], grid_pos_y_array[0]);

		for (int i = 0; i < grid_array_count; i++) {
			printf("第%d點 : \n", i);
			printf("(%d, %d)\n\n", x_grid_array[i], y_grid_array[i]);
		}
		grid_array_2_process();
		printf(" ------------------------------ \n");
		printf("the_min_x_grid : %d, the_max_x_grid : %d\n", the_min_x_grid, the_max_x_grid);
		printf("the_min_y_grid : %d, the_max_y_grid : %d\n", the_min_y_grid, the_max_y_grid);
		for (int i = the_min_y_grid; i <= the_max_y_grid; i++) {
			for (int j = the_min_x_grid; j <= the_max_x_grid; j++) {
				// 給3點資訊和最大最小的x和y
				draw_inside_grid(grid_pos_x_array, grid_pos_y_array, the_min_x_grid, the_max_x_grid, the_min_y_grid, the_max_y_grid, j, i);
			}
		}

		three_click_up = 0;
	}

	// fill_triangle(x_grid_array_2, y_grid_array_2, grid_array_count_2, the_min_y_grid, the_max_y_grid);

	m = ((float)grid_pos_y_2 - (float)grid_pos_y) / ((float)grid_pos_x_2 - (float)grid_pos_x);

	// midpoint line algo
	cout << "m: " << m << endl;

	if (midpoint_used == 1 || midpoint_and_anti_aliased_used == 1) {
		if (draw_grid == 1 && have_second_point >= 2) {
			//midpoint_algo_compute_grid(grid_pos_x, grid_pos_y, grid_pos_x_2, grid_pos_y_2);
		}
	}

	// anti-aliased line algo
	if (anti_aliased_used == 1) {
		if (draw_grid == 1 && have_second_point >= 2) {
			anti_aliased_algo(grid_pos_x, grid_pos_y, grid_pos_x_2, grid_pos_y_2);
		}
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

void init_grid_canvas() {
	draw_grid = 0;
	have_second_point = 0;
}

bool point_chaged = 0;

void find_left_point(int *two_points_info, int x1, int y1, int x2, int y2) {

	if (x1 > x2) {
		// need to change two points
		point_chaged = 1;

		two_points_info[0] = x2;
		two_points_info[1] = y2;
		two_points_info[2] = x1;
		two_points_info[3] = y1;
	}
	else {
		// not need to change two points

		two_points_info[0] = x1;
		two_points_info[1] = y1;
		two_points_info[2] = x2;
		two_points_info[3] = y2;
	}
}

void find_left_grid(int *grid_info, bool point_chaged) {

	if (point_chaged == 1) {
		grid_info[0] = x_min_2;
		grid_info[1] = x_max_2;
		grid_info[2] = y_min_2;
		grid_info[3] = y_max_2;
	}
	else {
		grid_info[0] = x_min;
		grid_info[1] = x_max;
		grid_info[2] = y_min;
		grid_info[3] = y_max;
	}
}

int x_min_basepoint;
int y_min_basepoint;
int x_max_basepoint;
int y_max_basepoint;

// round the number, when 0 < number
int round_number(float value) {
	int integer = value;
	float remain = value - integer;

	remain = abs(remain);
	if (0.5 <= remain) {
		remain = 1;
	}
	else {
		remain = 0;
	}

	int round_value = 0;
	if (value > 0) {
		round_value = integer + remain;
	}
	else if (value < 0) {
		round_value = integer - remain;
	}

	return round_value;
}

void writePixel(int pixel_x, int pixel_y) {
	glBegin(GL_POLYGON);
	glColor3f(0.0, 0.0, 0.0);
	glVertex2f(pixel_x * 2 + 1, pixel_y * 2 + 1);
	glVertex2f(pixel_x * 2 + 1, pixel_y * 2 - 1);
	glVertex2f(pixel_x * 2 - 1, pixel_y * 2 - 1);
	glVertex2f(pixel_x * 2 - 1, pixel_y * 2 + 1);
	glEnd();


	x_grid_array[grid_array_count] = pixel_x;
	y_grid_array[grid_array_count] = pixel_y;
	grid_array_count++;
}

void writePixel_2(int pixel_x, int pixel_y) {
	glBegin(GL_POLYGON);
	glColor3f(0.0, 0.0, 0.0);
	glVertex2f(pixel_x * 2 + 1, pixel_y * 2 + 1);
	glVertex2f(pixel_x * 2 + 1, pixel_y * 2 - 1);
	glVertex2f(pixel_x * 2 - 1, pixel_y * 2 - 1);
	glVertex2f(pixel_x * 2 - 1, pixel_y * 2 + 1);
	glEnd();
}

void writePixel_3(int pixel_x, int pixel_y) {
	glBegin(GL_POLYGON);
	glColor3f(1.0, 0.0, 0.0);
	glVertex2f(pixel_x * 2 + 1, pixel_y * 2 + 1);
	glVertex2f(pixel_x * 2 + 1, pixel_y * 2 - 1);
	glVertex2f(pixel_x * 2 - 1, pixel_y * 2 - 1);
	glVertex2f(pixel_x * 2 - 1, pixel_y * 2 + 1);
	glEnd();
}

void grid_array_2_process() {
	bool one_gate = 0;
	bool two_gate = 0;
	bool three_gate = 0;

	for (int i = 0; i < grid_array_count; i++) {

		if (x_grid_array[i] < the_min_x_grid) {
			the_min_x_grid = x_grid_array[i];
		}
		if (x_grid_array[i] > the_max_x_grid) {
			the_max_x_grid = x_grid_array[i];
		}
		if (y_grid_array[i] < the_min_y_grid) {
			the_min_y_grid = y_grid_array[i];
		}
		if (y_grid_array[i] > the_max_y_grid) {
			the_max_y_grid = y_grid_array[i];
		}

		if (x_grid_array[i] == grid_pos_x_array[0] && y_grid_array[i] == grid_pos_y_array[0] && one_gate == 0) {
			x_grid_array_2[grid_array_count_2] = x_grid_array[i];
			y_grid_array_2[grid_array_count_2] = y_grid_array[i];
			one_gate = 1;
			grid_array_count_2++;
		}else if (x_grid_array[i] == grid_pos_x_array[1] && y_grid_array[i] == grid_pos_y_array[1] && two_gate == 0) {
			x_grid_array_2[grid_array_count_2] = x_grid_array[i];
			y_grid_array_2[grid_array_count_2] = y_grid_array[i];
			two_gate = 1;
			grid_array_count_2++;
		}else if (x_grid_array[i] == grid_pos_x_array[2] && y_grid_array[i] == grid_pos_y_array[2] && three_gate == 0) {
			x_grid_array_2[grid_array_count_2] = x_grid_array[i];
			y_grid_array_2[grid_array_count_2] = y_grid_array[i];
			three_gate = 1;
			grid_array_count_2++;
		}
		else if (!(x_grid_array[i] == grid_pos_x_array[0] && y_grid_array[i] == grid_pos_y_array[0]) &&
					!(x_grid_array[i] == grid_pos_x_array[1] && y_grid_array[i] == grid_pos_y_array[1]) &&
					!(x_grid_array[i] == grid_pos_x_array[2] && y_grid_array[i] == grid_pos_y_array[2])){

			x_grid_array_2[grid_array_count_2] = x_grid_array[i];
			y_grid_array_2[grid_array_count_2] = y_grid_array[i];
			grid_array_count_2++;
		}

	}
}

//bool isInside(int *x_array, int *y_array, float m, float b, grid_x)

void draw_inside_grid(int *x_array, int *y_array, int min_x, int max_x, int min_y, int max_y, int grid_x, int grid_y) {
	float m_1, m_2, m_3;
	float b_1, b_2, b_3;
	bool line_1_greater_than_zero = 0,
		line_2_greater_than_zero = 0, 
		line_3_greater_than_zero = 0;

	bool line_1_greater_than_zero_2 = 0,
		line_2_greater_than_zero_2 = 0,
		line_3_greater_than_zero_2 = 0;

	m_1 = float(y_array[0] - y_array[1]) / float(x_array[0] - x_array[1]);
	m_2 = float(y_array[1] - y_array[2]) / float(x_array[1] - x_array[2]);
	m_3 = float(y_array[2] - y_array[0]) / float(x_array[2] - x_array[0]);
	b_1 = (-1) * m_1 * x_array[0] + y_array[0];
	b_2 = (-1) * m_2 * x_array[1] + y_array[1];
	b_3 = (-1) * m_3 * x_array[2] + y_array[2];

	cout << "m_1 : " << m_1 << ", b_1 : " << b_1 << endl;
	cout << "m_2 : " << m_2 << ", b_2 : " << b_2 << endl;
	cout << "m_3 : " << m_3 << ", b_3 : " << b_3 << endl;

	//bool line_1_inside = isInside(x_array, y_array, m_1, b_1, grid_x);
	if (x_array[0]  == x_array[1]) {
		if (x_array[2] > x_array[0]) {
			line_1_greater_than_zero = 1;
		}
		else if (x_array[2] < x_array[0]) {
			line_1_greater_than_zero = 0;
		}
		if (grid_x > x_array[0]) {
			line_1_greater_than_zero_2 = 1;
		}
		else if (grid_x < x_array[0]) {
			line_1_greater_than_zero_2 = 0;
		}
	}else{
		float value_1 = m_1 * x_array[2] - y_array[2] + b_1;
		if (value_1 > 0)
			line_1_greater_than_zero = 1;	// 右
		if (m_1 < 0) {
			line_1_greater_than_zero = !line_1_greater_than_zero;
		}
		float value_1_2 = m_1 * float(grid_x) - float(grid_y) + b_1;
		if (value_1_2 > 0)
			line_1_greater_than_zero_2 = 1;
		if (m_1 < 0) {
			line_1_greater_than_zero_2 = !line_1_greater_than_zero_2;
		}
	}
	
	if (x_array[1] == x_array[2]) {
		if (x_array[0] > x_array[1]) {
			line_2_greater_than_zero = 1;
		}
		else if (x_array[0] < x_array[1]) {
			line_2_greater_than_zero = 0;
		}
		if (grid_x > x_array[1]) {
			line_2_greater_than_zero_2 = 1;
		}
		else if (grid_x < x_array[1]) {
			line_2_greater_than_zero_2 = 0;
		}
	}
	else {
		float value_2 = m_2 * x_array[0] - y_array[0] + b_2;
		if (value_2 > 0)
			line_2_greater_than_zero = 1;
		if (m_2 < 0) {
			line_2_greater_than_zero = !line_2_greater_than_zero;
		}
		float value_2_2 = m_2 * float(grid_x) - float(grid_y) + b_2;
		if (value_2_2 > 0)
			line_2_greater_than_zero_2 = 1;
		if (m_2 < 0) {
			line_2_greater_than_zero_2 = !line_2_greater_than_zero_2;
		}
	}

	if (x_array[2] == x_array[0]) {
		if (x_array[1] > x_array[2]) {
			line_2_greater_than_zero = 1;
		}
		else if (x_array[1] < x_array[2]) {
			line_2_greater_than_zero = 0;
		}
		if (grid_x > x_array[2]) {
			line_2_greater_than_zero_2 = 1;
		}
		else if (grid_x < x_array[2]) {
			line_2_greater_than_zero_2 = 0;
		}
	}
	else {
		float value_3 = m_3 * x_array[1] - y_array[1] + b_3;
		if (value_3 > 0)
			line_3_greater_than_zero = 1;
		if (m_3 < 0)
			line_3_greater_than_zero = !line_3_greater_than_zero;
		float value_3_2 = m_3 * float(grid_x) - float(grid_y) + b_3;
		if (value_3_2 > 0)
			line_3_greater_than_zero_2 = 1;
		if (m_3 < 0)
			line_3_greater_than_zero_2 = !line_3_greater_than_zero_2;
	}
	
	cout << "line_1_greater_than_zero : " << line_1_greater_than_zero << endl;
	cout << "line_2_greater_than_zero : " << line_2_greater_than_zero << endl;
	cout << "line_3_greater_than_zero : " << line_3_greater_than_zero << endl;
	cout << "line_1_greater_than_zero_2 : " << line_1_greater_than_zero_2 << endl;
	cout << "line_2_greater_than_zero_2 : " << line_2_greater_than_zero_2 << endl;
	cout << "line_3_greater_than_zero_2 : " << line_3_greater_than_zero_2 << endl;
	if (line_1_greater_than_zero == line_1_greater_than_zero_2 &&
		line_2_greater_than_zero == line_2_greater_than_zero_2 &&
		line_3_greater_than_zero == line_3_greater_than_zero_2) {
		cout << "(" << grid_x << ",  " << grid_y << ") is inside." << endl;
		writePixel_2(grid_x, grid_y);
	}
	
}

void fill_triangle(int *x_array_2, int *y_array_2, int array_count_2, int min_y_grid, int max_y_grid) {
	
	for (int i = min_y_grid; i <= max_y_grid; i++) {
		int x_queue[100];
		int queue_count = 0;

		for (int j = 0; j < array_count_2; j++) {
			if (y_array_2[j] == i) {
				x_queue[queue_count] = x_array_2[j];
				queue_count++;
			}
		}

		// find x最大和最小
		int max_x = -88, min_x = 88;
		for (int d = 0; d < queue_count; d++) {
			if (x_queue[d] > max_x) {
				max_x = x_queue[d];
			}
			if (x_queue[d] < min_x) {
				min_x = x_queue[d];
			}
		}
		for (int d = min_x + 1; d < max_x; d++) {
			writePixel_2(d, i);
		}

	}
	
}

void midpoint_algo_compute_grid(int x1, int y1, int x2, int y2) {

	point_chaged = 0;

	/*	find left point		*/
	int two_points_info[4];
	find_left_point(two_points_info, x1, y1, x2, y2);
	/*	find left point		*/

	//	m is slope
	float dx = (float)two_points_info[2] - (float)two_points_info[0];
	float dy = (float)two_points_info[3] - (float)two_points_info[1];
	float m = dy / dx;

	float tan_valus_for_algo;
	float x = two_points_info[0], y = two_points_info[1];

	cout << "m_m : " << m << endl;

	// when slope between 1 and -1, include 0 slope
	if (-1 <= m && m <= 1) {
		if (midpoint_and_anti_aliased_used == 1) {
			draw_mid_antialiased_part1(two_points_info[0], two_points_info[2], two_points_info[1], two_points_info[3], dx, dy, m);
		}
		else {
			tan_valus_for_algo = dy / dx;

			while (x != two_points_info[2])
			{
				int round_y = round_number(y);	// 四捨五入
				writePixel(x, round_y);

				x++;
				y = y + tan_valus_for_algo;
			}
			writePixel(x, y);
		}
	}
	else if (1 < m) {
		tan_valus_for_algo = dx / dy;

		if (midpoint_and_anti_aliased_used == 1) {
			draw_mid_antialiased_part2(two_points_info[0], two_points_info[2], two_points_info[1], two_points_info[3], dx, dy);
		}
		else {
			while (y != two_points_info[3])
			{
				int round_x = round_number(x);
				writePixel(round_x, y);

				x = x + tan_valus_for_algo;
				y++;
			}
			writePixel(x, y);
		}
	}
	else if (m < -1) {
		if (midpoint_and_anti_aliased_used == 1) {
			draw_mid_antialiased_part4(two_points_info[0], two_points_info[2], two_points_info[1], two_points_info[3], dx, dy);
		}
		else {
			tan_valus_for_algo = dx / dy;

			while (y != two_points_info[3])
			{
				int round_x = round_number(x);	// 必須四捨五入，
				writePixel(round_x, y);

				x = x - tan_valus_for_algo;
				y--;
			}
			writePixel(x, y);
		}
	}
}

/*	Anti-aliased Line Generation Algorithm - positive_slope	*/
void anti_aliased_algo(int x1, int y1, int x2, int y2)
{
	point_chaged = 0;

	/*	find left point		*/
	int two_points_info[4];
	find_left_point(two_points_info, x1, y1, x2, y2);
	/*	find left point		*/

	/*	find left point min_mix		*/
	int grid_info[4];
	find_left_grid(grid_info, point_chaged);
	/*	find left point min_mix		*/

	//	m is slope
	float m = ((float)two_points_info[3] - (float)two_points_info[1]) / ((float)two_points_info[2] - (float)two_points_info[0]);

	float dx = (float)two_points_info[2] - (float)two_points_info[0];
	float dy = (float)two_points_info[3] - (float)two_points_info[1];
	float tan_valus_for_algo;
	float x = two_points_info[0], y = two_points_info[1];

	if (-1 <= m && m <= 1) {
		tan_valus_for_algo = dy / dx;
		float temp_front_y = floor(y);

		while (x != two_points_info[2]) {
			int floor_y = floor(y);
			float alpha = y - floor_y;

			x++;
			y = y + tan_valus_for_algo;

			// grid_info 存放網格資訊
			draw_grid_func(grid_info, 0, 1, alpha);

			/*	計算下一次的 grid位置	*/
			floor_y = floor(y);

			// when Positive slope or Zero slope
			if (0 <= m && m <= 1) {
				if (temp_front_y < floor_y) {
					grid_info[0] += 2;
					grid_info[1] += 2;
					grid_info[2] += 2;
					grid_info[3] += 2;

					temp_front_y = floor_y;
				}
				else {
					grid_info[0] += 2;
					grid_info[1] += 2;

					temp_front_y = floor_y;
				}
			}
			// when Negative slope
			if (-1 <= m && m < 0) {
				if (temp_front_y > floor_y) {
					grid_info[0] += 2;
					grid_info[1] += 2;
					grid_info[2] -= 2;
					grid_info[3] -= 2;

					temp_front_y = floor_y;
				}
				else {
					grid_info[0] += 2;
					grid_info[1] += 2;

					temp_front_y = floor_y;
				}
			}
			/*	計算下一次的 grid位置	*/
		}
	}
	else if (1 < m) {
		tan_valus_for_algo = dx / dy;
		float temp_front_x = floor(x);

		while (y != two_points_info[3]) {
			int floor_x = floor(x);
			float alpha = x - floor_x;

			x = x + tan_valus_for_algo;
			y++;

			// grid_info 存放網格資訊
			draw_grid_func(grid_info, 1, 0, alpha);

			/*	計算下一次的 grid位置	*/
			floor_x = floor(x);

			if (temp_front_x < floor_x) {
				grid_info[0] += 2;
				grid_info[1] += 2;
				grid_info[2] += 2;
				grid_info[3] += 2;

				temp_front_x = floor_x;
			}
			else {
				grid_info[2] += 2;
				grid_info[3] += 2;

				temp_front_x = floor_x;
			}
			/*	計算下一次的 grid位置	*/
		}
	}
	else if (m < -1) {
		tan_valus_for_algo = dx / dy;
		float temp_front_x = ceil(x);

		while (y != two_points_info[3]) {
			int ceil_x = ceil(x);
			float alpha = (x - ceil_x) * -1;

			x = x + tan_valus_for_algo;
			y--;

			// grid_info 存放網格資訊
			draw_grid_func(grid_info, 1, 0, alpha);

			/*	計算下一次的 grid位置	*/
			ceil_x = ceil(x);

			if (temp_front_x > ceil_x) {
				grid_info[0] += 2;
				grid_info[1] += 2;
				grid_info[2] -= 2;
				grid_info[3] -= 2;

				temp_front_x = ceil_x;
			}
			else {
				grid_info[2] -= 2;
				grid_info[3] -= 2;

				temp_front_x = ceil_x;
			}
			/*	計算下一次的 grid位置	*/
		}
	}
}
/*	Anti-aliased Line Generation Algorithm - positive_slope	*/

// 繪畫網格
// 如果x_trade == 1, 則以x軸多往右畫一格
void draw_grid_func(int *grid_info, bool x_trade, bool y_trade, float alpha) {

	// 做反鋸齒時
	if (x_trade || y_trade) {
		glBegin(GL_POLYGON);
		glColor4f(0.0, 0.0, 0.0, 1 - alpha);
		glVertex2f(grid_info[1], grid_info[3]);
		glVertex2f(grid_info[1], grid_info[2]);
		glVertex2f(grid_info[0], grid_info[2]);
		glVertex2f(grid_info[0], grid_info[3]);
		glEnd();

		int x_grid_offset = x_trade * 2;
		int y_grid_offset = y_trade * 2;
		glBegin(GL_POLYGON);
		glColor4f(0.0, 0.0, 0.0, alpha);
		glVertex2f(grid_info[1] + x_grid_offset, grid_info[3] + y_grid_offset);
		glVertex2f(grid_info[1] + x_grid_offset, grid_info[2] + y_grid_offset);
		glVertex2f(grid_info[0] + x_grid_offset, grid_info[2] + y_grid_offset);
		glVertex2f(grid_info[0] + x_grid_offset, grid_info[3] + y_grid_offset);
		glEnd();
	}
	else {
		glBegin(GL_POLYGON);
		glColor3f(0.0, 0.0, 0.0);
		glVertex2f(grid_info[1], grid_info[3]);
		glVertex2f(grid_info[1], grid_info[2]);
		glVertex2f(grid_info[0], grid_info[2]);
		glVertex2f(grid_info[0], grid_info[3]);
		glEnd();
	}
}

/*	Midpoint + Anti-aliased Line Generation Algorithm	*/
void draw_mid_antialiased_part1(float x, float x2, float y, float y2, float dx, float dy, float m) {
	float tan_valus_for_algo = dy / dx;
	float temp_front_y = floor(y);

	while (x != x2) {
		int floor_y = floor(y);
		float alpha = y - floor_y;

		x++;
		y = y + tan_valus_for_algo;

		// x, floor_y
		glBegin(GL_POLYGON);
		glColor4f(0.0, 0.0, 0.0, 1 - alpha);
		glVertex2f(x_max_basepoint, y_max_basepoint);
		glVertex2f(x_max_basepoint, y_min_basepoint);
		glVertex2f(x_min_basepoint, y_min_basepoint);
		glVertex2f(x_min_basepoint, y_max_basepoint);
		glEnd();

		// x, floor_y+1
		glBegin(GL_POLYGON);
		glColor4f(0.0, 0.0, 0.0, alpha);
		glVertex2f(x_max_basepoint, y_max_basepoint + 2);
		glVertex2f(x_max_basepoint, y_min_basepoint + 2);
		glVertex2f(x_min_basepoint, y_min_basepoint + 2);
		glVertex2f(x_min_basepoint, y_max_basepoint + 2);
		glEnd();

		/*	計算下一次的 grid位置	*/
		floor_y = floor(y);

		/*	計算下一次的 grid位置	*/
		// when Positive slope or Zero slope
		if (0 <= m && m <= 1) {
			if (temp_front_y < floor_y) {
				x_min_basepoint += 2;
				x_max_basepoint += 2;
				y_min_basepoint += 2;
				y_max_basepoint += 2;

				temp_front_y = floor_y;
			}
			else {
				x_min_basepoint += 2;
				x_max_basepoint += 2;

				temp_front_y = floor_y;
			}
		}
		// when Negative slope
		if (-1 <= m && m < 0) {
			if (temp_front_y > floor_y) {
				x_min_basepoint += 2;
				x_max_basepoint += 2;
				y_min_basepoint -= 2;
				y_max_basepoint -= 2;

				temp_front_y = floor_y;
			}
			else {
				x_min_basepoint += 2;
				x_max_basepoint += 2;

				temp_front_y = floor_y;
			}
		}
		/*	計算下一次的 grid位置	*/
	}
}

void draw_mid_antialiased_part2(float x, float x2, float y, float y2, float dx, float dy) {
	float tan_valus_for_algo = dx / dy;
	float temp_front_x = floor(x);

	while (y != y2) {
		int floor_x = floor(x);
		float alpha = x - floor_x;

		x = x + tan_valus_for_algo;
		y++;

		// x, floor_y
		glBegin(GL_POLYGON);
		glColor4f(0.0, 0.0, 0.0, 1 - alpha);
		glVertex2f(x_max_basepoint, y_max_basepoint);
		glVertex2f(x_max_basepoint, y_min_basepoint);
		glVertex2f(x_min_basepoint, y_min_basepoint);
		glVertex2f(x_min_basepoint, y_max_basepoint);
		glEnd();

		// x, floor_y+1
		glBegin(GL_POLYGON);
		glColor4f(0.0, 0.0, 0.0, alpha);
		glVertex2f(x_max_basepoint + 2, y_max_basepoint);
		glVertex2f(x_max_basepoint + 2, y_min_basepoint);
		glVertex2f(x_min_basepoint + 2, y_min_basepoint);
		glVertex2f(x_min_basepoint + 2, y_max_basepoint);
		glEnd();

		/*	計算下一次的 grid位置	*/
		floor_x = floor(x);

		if (temp_front_x < floor_x) {
			x_min_basepoint += 2;
			x_max_basepoint += 2;
			y_min_basepoint += 2;
			y_max_basepoint += 2;

			temp_front_x = floor_x;
		}
		else {
			y_min_basepoint += 2;
			y_max_basepoint += 2;

			temp_front_x = floor_x;
		}
		/*	計算下一次的 grid位置	*/
	}
}

void draw_mid_antialiased_part3(float x, float x2, float y, float y2, float dx, float dy) {
	float tan_valus_for_algo = dy / dx;
	float temp_front_y = floor(y);

	while (x != x2) {
		int floor_y = floor(y);
		float alpha = y - floor_y;

		x++;
		y = y + tan_valus_for_algo;

		// x, floor_y
		glBegin(GL_POLYGON);
		glColor4f(0.0, 0.0, 0.0, 1 - alpha);
		glVertex2f(x_max_basepoint, y_max_basepoint);
		glVertex2f(x_max_basepoint, y_min_basepoint);
		glVertex2f(x_min_basepoint, y_min_basepoint);
		glVertex2f(x_min_basepoint, y_max_basepoint);
		glEnd();

		// x, floor_y+1
		glBegin(GL_POLYGON);
		glColor4f(0.0, 0.0, 0.0, alpha);
		glVertex2f(x_max_basepoint, y_max_basepoint + 2);
		glVertex2f(x_max_basepoint, y_min_basepoint + 2);
		glVertex2f(x_min_basepoint, y_min_basepoint + 2);
		glVertex2f(x_min_basepoint, y_max_basepoint + 2);
		glEnd();

		/*	計算下一次的 grid位置	*/
		floor_y = floor(y);

		if (temp_front_y > floor_y) {
			x_min_basepoint += 2;
			x_max_basepoint += 2;
			y_min_basepoint -= 2;
			y_max_basepoint -= 2;

			temp_front_y = floor_y;
		}
		else {
			x_min_basepoint += 2;
			x_max_basepoint += 2;

			temp_front_y = floor_y;
		}
		/*	計算下一次的 grid位置	*/
	}
}

void draw_mid_antialiased_part4(float x, float x2, float y, float y2, float dx, float dy) {
	float tan_valus_for_algo = dx / dy;
	float temp_front_x = ceil(x);

	while (y != y2) {
		int ceil_x = ceil(x);
		float alpha = (x - ceil_x) * -1;

		x = x + tan_valus_for_algo;
		y--;

		// ceil_x, y
		glBegin(GL_POLYGON);
		glColor4f(0.0, 0.0, 0.0, 1 - alpha);
		glVertex2f(x_max_basepoint, y_max_basepoint);
		glVertex2f(x_max_basepoint, y_min_basepoint);
		glVertex2f(x_min_basepoint, y_min_basepoint);
		glVertex2f(x_min_basepoint, y_max_basepoint);
		glEnd();

		// ceil_x, y
		glBegin(GL_POLYGON);
		glColor4f(0.0, 0.0, 0.0, alpha);
		glVertex2f(x_max_basepoint + 2, y_max_basepoint);
		glVertex2f(x_max_basepoint + 2, y_min_basepoint);
		glVertex2f(x_min_basepoint + 2, y_min_basepoint);
		glVertex2f(x_min_basepoint + 2, y_max_basepoint);
		glEnd();

		/*	計算下一次的 grid位置	*/
		ceil_x = ceil(x);

		if (temp_front_x > ceil_x) {
			x_min_basepoint += 2;
			x_max_basepoint += 2;
			y_min_basepoint -= 2;
			y_max_basepoint -= 2;

			temp_front_x = ceil_x;
		}
		else {
			y_min_basepoint -= 2;
			y_max_basepoint -= 2;

			temp_front_x = ceil_x;
		}
		/*	計算下一次的 grid位置	*/
	}
}
/*	Midpoint + Anti-aliased Line Generation Algorithm	*/

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
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
