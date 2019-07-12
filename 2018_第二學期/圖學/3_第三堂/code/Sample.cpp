#include <math.h>
#include <GL/glut.h>
#include<stdio.h>

float tran_x = 0.0f;
float tran_y = 0.0f;
float tran_z = 0.0f;

float rotateX = 0.0f;
float rotateY = 0.0f;


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

	glRotatef(rotateX, 1.0, 0.0, 0.0);
	glRotatef(rotateY, 0.0, 1.0, 0.0);
	//glTranslatef(tx, ty, tz);
	glTranslatef(tran_x, tran_y, tran_z);

	//畫出cube
	glColor3f(0.0, 0.0, 1.0);
	glutSolidCube(1.0f);
	//glutSwapBuffers();

	//旋轉

	//位移


	glFlush();
	glutSwapBuffers();
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

/*void SpecialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)   rotateX -= 5.0f;
	if (key == GLUT_KEY_DOWN)  rotateX += 5.0f;
	if (key == GLUT_KEY_LEFT)  rotateY -= 5.0f;
	if (key == GLUT_KEY_RIGHT)  rotateY += 5.0f;

	if (rotateX > 356.0f)  rotateX = 0.0f;
	if (rotateX < -1.0f)  rotateX = 355.0f;
	if (rotateY > 356.0f)  rotateY = 0.0f;
	if (rotateY < -1.0f)  rotateY = 355.0f;

}*/

void NormalKeyHandler(unsigned char key, int x, int y)
{
	switch (key)
	{
		// transform
	case 'w':
		tran_y += 1.0f;
		break;
	case 's':
		tran_y -= 1.0f;
		break;
	case 'a':
		tran_x -= 1.0f;
		break;
	case 'd':
		tran_x += 1.0f;
		break;

		// rotation
	case 'i':
		rotateX -= 5.0f;
		break;
	case 'k':
		rotateX += 5.0f;
		break;
	case 'j':
		rotateY -= 5.0f;
		break;
	case 'l':
		rotateY += 5.0f;
		break;
	default:
		break;
	}

	if (rotateX > 356.0f)  rotateX = 0.0f;
	if (rotateX < -1.0f)  rotateX = 355.0f;
	if (rotateY > 356.0f)  rotateY = 0.0f;
	if (rotateY < -1.0f)  rotateY = 355.0f;

	//刷新窗口
	glutPostRedisplay();
}

void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glFrustum(-1, 1, -1, 1, 1, 1000);
	glEnable(GL_DEPTH_TEST);
}


void keyboard(unsigned char key, int x, int y)
{
	//按鍵操作
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

	glutDisplayFunc(RenderScence);
	glutKeyboardFunc(NormalKeyHandler);
	//glutSpecialFunc(SpecialKeys);


	glutMainLoop();

	return 0;
}