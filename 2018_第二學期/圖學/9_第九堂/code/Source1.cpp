#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define X .525731112119133606 //使原點到每個頂點的距離均為1.0
#define Z .850650808352039932

GLfloat LightPosition[] = { 5.0f, 5.0f, 0.0f, 1.0f }; //光源位置
GLfloat LightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f }; //環境光參數
GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //漫射光參數

GLfloat xrot; //繞軸旋轉
GLfloat yrot;
GLfloat zrot;

void draw_Icosahedron(int divide_times, int ShadeModel);
void NormalKeyHandler(unsigned char key, int x, int y);

int width = 600; //窗口大小
int height = 200;

bool fill_shape = 0;

int subdivision_times = 1;

//頂點座標
static GLfloat vdata[12][3] = {
	{ -X, 0.0, Z }, { X, 0.0, Z }, { -X, 0.0, -Z }, { X, 0.0, -Z },
	{ 0.0, Z, X }, { 0.0, Z, -X }, { 0.0, -Z, X }, { 0.0, -Z, -X },
	{ Z, X, 0.0 }, { -Z, X, 0.0 }, { Z, -X, 0.0 }, { -Z, -X, 0.0 }
};

//三角形的面
static GLuint tindices[20][3] = {
	{1, 4, 0}, {4, 9, 0}, {4, 5, 9}, {8, 5, 4}, {1, 8, 4},
	{1, 10, 8}, {10, 3, 8}, {8, 3, 5}, {3, 2, 5}, {3, 7, 2},
	{3, 10, 7}, {10, 6, 7}, {6, 11, 7}, {6, 0, 11}, {6, 1, 0},
	{10, 1, 6}, {11, 0, 9}, {2, 11, 9}, {5, 2, 9}, {11, 2, 7}
};

void normalize(float v[3]); //計算兩個向量的規範化向量積
void normcrossprod(float v1[3], float v2[3], float out[3]);
void drawtriangle(float *v1, float *v2, float *v3); //畫三角形
void subdivide(float *v1, float *v2, float *v3); //單次劃分三角形
void subdivide(float *v1, float *v2, float *v3, long depth); //遞迴求解，多次劃分三角形(通過depth控制)

void normalize(float v[3])
{
	GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); //三個向量的模
	if (0 == d)
	{
		printf("zero length vector!");
		return;
	}
	//單位向量
	v[0] /= d;
	v[1] /= d;
	v[2] /= d;
}

void normcrossprod(float v1[3], float v2[3], float out[3])
{
	out[0] = v1[1] * v2[2] - v1[2] * v1[1];
	out[1] = v1[2] * v2[0] - v1[0] * v1[2];
	out[2] = v1[0] * v2[1] - v1[1] * v1[0];
	normalize(out);
}

void drawtriangle(float *v1, float *v2, float *v3)
{
	glBegin(GL_TRIANGLES);
	glNormal3fv(v1);
	glVertex3fv(v1);
	glNormal3fv(v2);
	glVertex3fv(v2);
	glNormal3fv(v3);
	glVertex3fv(v3);
	glEnd();
}

void subdivide(float *v1, float *v2, float *v3) //非遞迴的單次細分
{
	GLfloat v12[3], v23[3], v31[3];
	GLint i;
	for (i = 0; i < 3; i++)
	{
		v12[i] = (v1[i] + v2[i]) / 2.0;
		v23[i] = (v2[i] + v3[i]) / 2.0;
		v31[i] = (v3[i] + v1[i]) / 2.0;
	}
	normalize(v12);
	normalize(v23);
	normalize(v31);
	drawtriangle(v1, v12, v31);
	drawtriangle(v2, v23, v12);
	drawtriangle(v3, v31, v23);
	drawtriangle(v12, v23, v31);
}

void subdivide(float *v1, float *v2, float *v3, long depth) //遞迴的depth次細分
{
	GLfloat v12[3], v23[3], v31[3];
	GLint i;
	if (0 == depth)
	{
		drawtriangle(v1, v2, v3);
		return;
	}
	for (i = 0; i < 3; i++)
	{
		v12[i] = (v1[i] + v2[i]) / 2.0;
		v23[i] = (v2[i] + v3[i]) / 2.0;
		v31[i] = (v3[i] + v1[i]) / 2.0;
	}
	normalize(v12);
	normalize(v23);
	normalize(v31);
	subdivide(v1, v12, v31, depth - 1);
	subdivide(v2, v23, v12, depth - 1);
	subdivide(v3, v31, v23, depth - 1);
	subdivide(v12, v23, v31, depth - 1);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清除顏色和深度緩存
	glLoadIdentity(); //重製當前的模型觀察矩陣
	glTranslatef(0.0f, 0.0f, -5.0f); //移入螢幕5個單位

	glColor3f(0.0, 1.0, 0.0);
	glViewport(0 + 50, 50, (width / 3) - 100, height - 100);
	glLoadIdentity();
	draw_Icosahedron(0, 0);
  
	glColor3f(0.0, 0.0, 1.0);
	glViewport(width/3 + 50, 50, (width / 3) - 100, height - 100);
	glLoadIdentity();
	draw_Icosahedron(0, 1);

	glColor3f(1.0, 0.0, 0.0);
	glViewport(width / 3 * 2 + 50, 50, (width / 3) - 100, height - 100);
	glLoadIdentity();
	draw_Icosahedron(subdivision_times, 1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	
	/*glBegin(GL_TRIANGLES);
	for (int i = 0; i < 20; i++)
	{
		glVertex3fv(&vdata[tindices[i][0]][0]);
		glVertex3fv(&vdata[tindices[i][1]][0]);
		glVertex3fv(&vdata[tindices[i][2]][0]);
	}
	glEnd();*/
	
	glFlush();
	glutSwapBuffers();
}

void draw_Icosahedron(int divide_times, int ShadeModel) {
	if (fill_shape == 0) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //多邊形的顯示方式
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //多邊形的顯示方式
	}
	
	glRotatef(xrot, 1.0f, 0.0f, 0.0f); //繞軸旋轉
	glRotatef(yrot, 0.0f, 1.0f, 0.0f); //繞軸旋轉
	glRotatef(zrot, 0.0f, 0.0f, 1.0f); //繞軸旋轉

	if (ShadeModel == 0) {
		glShadeModel(GL_FLAT); //啟用陰影平滑
	} else {
		glShadeModel(GL_SMOOTH); //啟用陰影平滑
	}
	for (int i = 0; i < 20; i++)
	{
		/*
		生成表面的法線向量
		*/
		GLfloat d1[3], d2[3], norm[3];
		for (int j = 0; j < 3; j++)
		{
			d1[j] = vdata[tindices[i][0]][j] - vdata[tindices[i][1]][j];
			d2[j] = vdata[tindices[i][1]][j] - vdata[tindices[i][2]][j];
		}
		normcrossprod(d1, d2, norm);
		glNormal3fv(norm);
		/*drawtriangle(&vdata[tindices[i][0]][0],
					 &vdata[tindices[i][1]][0],
					 &vdata[tindices[i][2]][0]);*/
					 /*glBegin(GL_TRIANGLES);
						 glNormal3fv(&vdata[tindices[i][0]][0]);
						 glVertex3fv(&vdata[tindices[i][0]][0]);
						 glNormal3fv(&vdata[tindices[i][1]][0]);
						 glVertex3fv(&vdata[tindices[i][1]][0]);
						 glNormal3fv(&vdata[tindices[i][2]][0]);
						 glVertex3fv(&vdata[tindices[i][2]][0]);
					 glEnd();*/
		subdivide(&vdata[tindices[i][0]][0],
			&vdata[tindices[i][1]][0],
			&vdata[tindices[i][2]][0],
			divide_times //劃分次數
		);
	}

	glutPostRedisplay();
}


void NormalKeyHandler(unsigned char key, int x, int y)
{
	switch (key)
	{
		// rotation
	case 'o':
		xrot += 5.0f;
		break;
	case 'l':
		xrot -= 5.0f;
		break;
	case 'i':
		yrot += 5.0f;
		break;
	case 'k':
		yrot -= 5.0f;
		break;
	case 'u':
		zrot += 5.0f;
		break;
	case 'j':
		zrot -= 5.0f;
		break;

	case 'n':
		fill_shape = 0;
		break;
	case 'm':
		fill_shape = 1;
		break;

	case '+':
		subdivision_times += 1;
		break;
	case '-':
		if(subdivision_times >= 1)
			subdivision_times -= 1;
		break;
		
	default:
		break;
	}

	if (xrot > 356.0f)  xrot = 0.0f;
	if (xrot < -1.0f)  xrot = 355.0f;
	if (yrot > 356.0f)  yrot = 0.0f;
	if (yrot < -1.0f)  yrot = 355.0f;
	if (zrot > 356.0f)  zrot = 0.0f;
	if (zrot < -1.0f)  zrot = 355.0f;

	//刷新窗口
	glutPostRedisplay();
}

GLfloat mat_specular[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat mat_diffuse[] = { 0.8, 0.6, 0.4, 1.0 };
GLfloat mat_ambient[] = { 0.8, 0.6, 0.4, 1.0 };
GLfloat mat_shininess = { 20.0 };

void init()
{
	glMatrixMode(GL_PROJECTION); //選擇投影矩陣
	glLoadIdentity(); //重置投影矩陣
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f); //透視效果
	glMatrixMode(GL_MODELVIEW); //選擇模型觀察矩陣
	glLoadIdentity(); //重置模型觀察矩陣

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	glShadeModel(GL_SMOOTH); //啟用陰影平滑
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //設置背景顏色
	glClearDepth(1.0f); //設置深度緩存
	glEnable(GL_DEPTH_TEST); //啟用深度測試
	glDepthFunc(GL_LESS); //所做測試的類型
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); //透視修正

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient); //設置環境光
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse); //設置漫射光
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition); //設置光源位置
	glEnable(GL_LIGHT1); //啟用1號光源
	glEnable(GL_LIGHTING); //開啟光源
}

void reshape(int w, int h) //當視窗大小改變時呼叫
{
	width = w;
	height = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h); //設置視窗的大小

	init();

	gluOrtho2D(0.0f, (GLdouble)w, 0.0, (GLdouble)h);  //建立二維可視區域
}

int main(int argv, char **argc)
{
	glutInit(&argv, argc); //glut初始化
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); //設置顯示模式
	glutInitWindowSize(900, 300);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("二十面體");
	init();
	glutDisplayFunc(display); //測試繪圖函數
	glutKeyboardFunc(NormalKeyHandler);
	glutReshapeFunc(reshape); //註冊視窗大小改變函數
	glutMainLoop(); //glut事件處理
	return 0;
}