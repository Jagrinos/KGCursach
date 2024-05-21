#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

#include <vector>
#include <cmath>
#include <iostream>

#define pi 3.1415926f

using namespace std;

bool textureMode = true;
bool lightMode = true;

double normal[3] = { 0,0,0 };

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;


	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist * cos(fi2) * cos(fi1),
			camDist * cos(fi2) * sin(fi1),
			camDist * sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}


	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);


		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale * 0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale * 1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL* ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01 * dx;
		camera.fi2 += -0.01 * dy;
	}


	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02 * dy);
	}


}

void mouseWheelEvent(OpenGL* ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01 * delta;

}

void keyDownEvent(OpenGL* ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL* ogl, int key)
{

}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL* ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("max.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH);


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}



void place(double height) {

	double a[3] = { 7,18,height };
	double b[3] = { 8,11,height };
	double c[3] = { 13,10,height };
	double d[3] = { 14,4,height };
	double e[3] = { 5,0,height };
	double f[3] = { 0,5,height };
	double g[3] = { 5,9,height };
	double h[3] = { 3,13,height };
	//glBindTexture(GL_TEXTURE_2D, texId);
	glNormal3f(0, 0, height == 0 ? -1 : 1);

	
	glBegin(GL_QUADS);
	glColor3d(0.5, 0.1, 0.3);
	glVertex3dv(a);
	glVertex3dv(b);
	glVertex3dv(g);
	glVertex3dv(h);

	glVertex3dv(b);
	glVertex3dv(c);
	glVertex3dv(d);
	glVertex3dv(g);

	glEnd();

	//glBindTexture(GL_TEXTURE_2D, texId);

	const int numPoints = 1000;
	double radius = sqrt(2.5 * 2.5 * 2);

	glBegin(GL_POLYGON);

	glVertex3dv(g);
	glVertex3dv(f);

	glVertex3d(5, 0, height);

	for (int i = 0; i < numPoints; i++) {

		float theta = 2.0f * pi * float(i) / float(numPoints);
		float x = radius * cosf(theta) + 2.5;
		float y = radius * sinf(theta) + 2.5;
		if (y <= 5 && x <= 5)
			glVertex3d(x, y, height);
	}

	glEnd();

	radius = sqrt(34);
	double xc = 10.8, yc = -0.9;
	//glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_POLYGON);
	glVertex3dv(g);
	glVertex3dv(d);

	for (int i = 0; i < numPoints; i++) {
		float theta = 2.0f * pi * float(i) / float(numPoints);
		float x = radius * cosf(theta) + xc;
		float y = radius * sinf(theta) + yc;
		if (x < 14 && y > 0 && x > 5)
			glVertex3d(x, y, height);
	}
	glVertex3dv(e);
	glEnd();
}



void raz_pol(vector <double> a, vector <double> b) {
	normal[0] = a[1] * b[2] - b[1] * a[2];
	normal[1] = (-a[0] * b[2] + b[0] * a[2]);
	normal[2] = a[0] * b[1] - b[0] * a[1];
}

void walls(double height) {
	double a[3] = { 7,18,height };
	double a_l[3] = { 7,18,0 };
	double b[3] = { 8,11,height };
	double b_l[3] = { 8,11,0 };
	double c[3] = { 13,10,height };
	double c_l[3] = { 13,10,0 };
	double d[3] = { 14,4,height };
	double d_l[3] = { 14,4,0 };
	double e[3] = { 5,0,height };
	double e_l[3] = { 5,0,0 };
	double f[3] = { 0,5,height };
	double f_l[3] = { 0,5,0 };
	double g[3] = { 5,9,height };
	double g_l[3] = { 5,9,0 };
	double h[3] = { 3,13,height };
	double h_l[3] = { 3,13,0 };
	;
	glBegin(GL_QUADS);
	glColor3d(1, 0.3, 0.3);

	vector<double> frst = { 1,-7,0 };
	vector<double> secd = { 0,0, height };
	raz_pol(secd, frst);
	//glBindTexture(GL_TEXTURE_2D, texId);
	glNormal3f(normal[0], normal[1], normal[2]);

	glVertex3dv(a);
	glVertex3dv(a_l);
	glVertex3dv(b_l);
	glVertex3dv(b);

	frst = { 5,-1,0 };
	secd = { 0,0, height };
	raz_pol(secd, frst);
	glNormal3f(normal[0], normal[1], normal[2]);

	glVertex3dv(b);
	glVertex3dv(b_l);
	glVertex3dv(c_l);
	glVertex3dv(c);

	frst = { 1,-6,0 };
	secd = { 0,0, height };
	raz_pol(secd, frst);
	glNormal3f(normal[0], normal[1], normal[2]);

	glVertex3dv(c);
	glVertex3dv(c_l);
	glVertex3dv(d_l);
	glVertex3dv(d);


	frst = { -2,4,0 };
	secd = { 0,0, height };
	raz_pol(secd, frst);
	glNormal3f(normal[0], normal[1], normal[2]);

	glVertex3dv(g);
	glVertex3dv(g_l);
	glVertex3dv(h_l);
	glVertex3dv(h);

	frst = { 4,5,0 };
	secd = { 0,0, height };
	raz_pol(secd, frst);
	glNormal3f(normal[0], normal[1], normal[2]);

	glVertex3dv(h);
	glVertex3dv(h_l);
	glVertex3dv(a_l);
	glVertex3dv(a);

	frst = { 5,4,0 };
	secd = { 0,0, height };
	raz_pol(secd, frst);
	glNormal3f(normal[0], normal[1], normal[2]);

	glVertex3dv(g);
	glVertex3dv(g_l);
	glVertex3dv(f_l);
	glVertex3dv(f);

	glEnd();

	//glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_TRIANGLE_STRIP);

	glVertex3dv(f);
	glVertex3dv(f_l);

	const int numPoints = 1000;
	double radius = sqrt(2.5 * 2.5 * 2);

	float x_l = 0;
	float y_l = 5;

	for (int i = 0; i < numPoints; i++) {
		float theta = 2.0f * pi * float(i) / float(numPoints);
		float x = radius * cosf(theta) + 2.5;
		float y = radius * sinf(theta) + 2.5;
		if (y < 5 && x < 4.99) {
			frst = { x - x_l, y - y_l, 0 };
			secd = { 0,0, -height };
			raz_pol(secd, frst);
			glNormal3f(normal[0], normal[1], normal[2]);

			frst = { x_l - x, y_l - y, 0 };
			secd = { 0,0, height };
			raz_pol(secd, frst);
			glNormal3f(normal[0], normal[1], normal[2]);

			glVertex3d(x, y, height);
			glVertex3d(x, y, 0);

			x_l = x;
			y_l = y;
		}
	}
	glVertex3dv(e);
	glVertex3dv(e_l);
	glEnd();
	//glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_TRIANGLE_STRIP);

	radius = sqrt(34);
	double xc = 10.8, yc = -0.9;

	glVertex3dv(d);
	glVertex3dv(d_l);

	x_l = 14;
	y_l = 4;

	for (int i = 0; i < numPoints; i++) {
		float theta = 2.0f * pi * float(i) / float(numPoints);
		float x = radius * cosf(theta) + xc;
		float y = radius * sinf(theta) + yc;
		if (x < 14 && y > 0)
		{
			frst = { x_l - x, y_l - y, 0 };
			secd = { 0,0, -height };
			raz_pol(secd, frst);
			glNormal3f(normal[0], normal[1], normal[2]);

			frst = { x - x_l, y - y_l, 0 };
			secd = { 0,0, height };
			raz_pol(secd, frst);
			glNormal3f(normal[0], normal[1], normal[2]);


			glVertex3d(x, y, height);
			glVertex3d(x, y, 0);

			x_l = x;
			y_l = y;
		}
	}

	glVertex3dv(e);
	glVertex3dv(e_l);

	glEnd();
}


void Render(OpenGL* ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  

	int height = 1;

	place(0);
	walls(height);
	place(height);

	//����� ��������� ���������� ��������


   //��������� ������ ������


	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	//(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}