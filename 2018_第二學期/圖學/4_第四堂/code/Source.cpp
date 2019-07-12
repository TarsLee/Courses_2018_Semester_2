#include <math.h>
#include <GL/glut.h>
#include<stdio.h>

#define M_PI 3.14159265358979323846

// 運算矩陣
GLfloat multiMatrix[16];

/*	for normal keyboard	*/
GLfloat gl_tran_x = 0;
GLfloat gl_tran_y = 0;
GLfloat gl_tran_z = 0;

GLfloat gl_rot_x = 0;
GLfloat gl_rot_y = 0;
GLfloat gl_rot_z = 0;

GLfloat gl_sca = 1;

/*	for mouse	*/
float mouse_X = 0;
float mouse_Y = 0;
float mouse_Z = 0;

GLfloat mouse_Self_angle = 0;

bool use_mouse_rotate = 0;

// initial matrix
void MatrixReset() {
	multiMatrix[0] = 1; multiMatrix[1] = 0; multiMatrix[2] = 0; multiMatrix[3] = 0;
	multiMatrix[4] = 0; multiMatrix[5] = 1; multiMatrix[6] = 0; multiMatrix[7] = 0;
	multiMatrix[8] = 0; multiMatrix[9] = 0; multiMatrix[10] = 1; multiMatrix[11] = 0;
	multiMatrix[12] = 0; multiMatrix[13] = 0; multiMatrix[14] = 0; multiMatrix[15] = 1;
}

// cube matrix reset
void AllReset() {
	// for x,y,z rotate
	gl_tran_x = 0;
	gl_tran_y = 0;
	gl_tran_z = 0;

	// for x,y,z rotate
	gl_rot_x = 0;
	gl_rot_y = 0;
	gl_rot_z = 0;

	// for mouse location
	mouse_X = 0;
	mouse_Y = 0;
	mouse_Z = 0;

	// for scale
	gl_sca = 1;
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
		

	if (use_mouse_rotate == 1) {
		// for mouse rotate

		//if axis is not already normalised then uncomment this
		double magnitude = sqrt(x * x + y * y + z * z);
		if (magnitude == 0) {
			return;
		}
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;

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

	}else {
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

}

void ScaleMatrix() {
	MatrixReset();
	multiMatrix[0] = gl_sca;
	multiMatrix[5] = gl_sca;
	multiMatrix[10] = gl_sca;
	glMultMatrixf(multiMatrix);
}

void RenderScence(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 5.0f, 0, 0, 0, 0, 1, 0);

	//畫X軸Y軸
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(10, 0, 0);
	glVertex3f(-10, 0, 0);
	glVertex3f(0, 10, 0);
	glVertex3f(0, -10, 0);
	glVertex3f(0, 0, 10);
	glVertex3f(0, 0, -10);
	glEnd();

	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(mouse_X, mouse_Y, 0);
	glPointSize(10.0);
	glEnd();

	glPointSize(5.0);
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_POINTS);
	glVertex2f(mouse_X, mouse_Y);
	glEnd();

	//放大縮小
	ScaleMatrix();

	//旋轉
	RotateMatrix(mouse_Self_angle, mouse_X, mouse_Y, mouse_Z);

	RotateMatrix(gl_rot_x, 1.0, 0.0, 0.0);
	RotateMatrix(gl_rot_y, 0.0, 1.0, 0.0);
	RotateMatrix(gl_rot_z, 0.0, 0.0, 1.0);
	//glRotatef(gl_rot_x, 1.0, 0.0, 0.0);
	//glRotatef(gl_rot_y, 0.0, 1.0, 0.0);

	//位移
	TranslateMatrix(gl_tran_x, gl_tran_y, gl_tran_z);
	//glTranslatef(gl_tran_x, gl_tran_y, gl_tran_z);


	//畫出cube
	glColor3f(0.0, 0.0, 1.0);
	glutSolidCube(1.0f);


	glFlush();
	glutSwapBuffers();
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

	// self-axis
	case 'c':
		use_mouse_rotate = 1;
		mouse_Self_angle += 1.0f;
		break;
	case 'v':
		use_mouse_rotate = 1;
		mouse_Self_angle -= 1.0f;
		break;

	// scale
	case 'z':
		gl_sca += 1.0f;
		break;
	case 'x':
		gl_sca -= 1.0f;
		break;

	//reset
	case 'r':
		AllReset();
		break;

	default:
		break;
	}

	if (gl_tran_x > 4.0f)  gl_tran_x = 4.0f;
	if (gl_tran_x < -4.0f)  gl_tran_x = -4.0f;
	if (gl_tran_y > 4.0f)  gl_tran_y = 4.0f;
	if (gl_tran_y < -4.0f)  gl_tran_y = -4.0f;
	if (gl_tran_z > 4.0f)  gl_tran_z = 4.0f;
	if (gl_tran_z < -4.0f)  gl_tran_z = -4.0f;

	if (gl_rot_x > 356.0f)  gl_rot_x = 0.0f;
	if (gl_rot_x < -1.0f)  gl_rot_x = 355.0f;
	if (gl_rot_y > 356.0f)  gl_rot_y = 0.0f;
	if (gl_rot_y < -1.0f)  gl_rot_y = 355.0f;
	if (gl_rot_z > 356.0f)  gl_rot_z = 0.0f;
	if (gl_rot_z < -1.0f)  gl_rot_z = 355.0f;

	if (gl_sca > 4.0f)  gl_sca = 4.0f;
	if (gl_sca < 1.0f)  gl_sca = 1.0f;

	//刷新窗口
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	//printf("x: %f, y: %f\n", mouse_X, mouse_Y);
	if (button == GLUT_LEFT_BUTTON) {
		mouse_X = float(x - 250) / 50;
		mouse_Y = float(250 - y) / 50;

		printf("x: %f, y: %f\n", mouse_X, mouse_Y);
	}

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

void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glFrustum(-1, 1, -1, 1, 1, 1000);
	glEnable(GL_DEPTH_TEST);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Cube");

	init();
	SetupRC();

	use_mouse_rotate = 0;
	MatrixReset();
	glutKeyboardFunc(NormalKeyHandler);
	glutDisplayFunc(RenderScence);
	//glutSpecialFunc(SpecialKeys);


	glutMouseFunc(Mouse);

	glutMainLoop();

	return 0;
}