#include <cstdlib>
#include <ctime>
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
#include <cmath>
#include <string>
#define PI 3.14159265

bool textureMode = true;
bool lightMode = true;
bool textureReplace = true;
bool orb = true;
bool st = false;
bool rotation = false;
bool real = false;

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
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
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}

}  camera;   //создаем объект камеры


//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(0, 0, 0);
	}

	//рисует сферу и линии под источником света, вызывается движком
	void DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);
		glColor3d(0.9, 0.8, 0);
	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 1.0 };
		GLfloat dif[] = { 0.8, 0.8, 0.8, 1.0 };
		GLfloat spec[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света

//старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL* ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01 * dx;
		camera.fi2 += -0.01 * dy;
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
	if (key == 'T')
		orb = !orb;
	if (key == 'S') 
		st = !st;
	if (key == 'U')
		rotation = !rotation;
	if (key == 'G')
		real = !real;
}

GLuint Sun;
GLuint Mercury;
GLuint Venus;
GLuint Earth;
GLuint Mars;
GLuint Jupiter;
GLuint Saturn;
GLuint SaturnRing;
GLuint Uranus;
GLuint UranusRing;
GLuint Neptune;
GLuint Moon;
GLuint Asteroid;

//выполняется перед первым рендером
void initRender(OpenGL* ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);


	//массив трехбайтных элементов  (R G B)
	{RGBTRIPLE* texarray;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("Earth.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	//генерируем ИД для текстуры
	glGenTextures(1, &Earth);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, Earth);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//отчистка памяти
	free(texCharArray);
	free(texarray);

	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	{
		RGBTRIPLE* texarray2;
		char* texCharArray2;
		int texW2, texH2;
		OpenGL::LoadBMP("Mercury.bmp", &texW2, &texH2, &texarray2);
		OpenGL::RGBtoChar(texarray2, texW2, texH2, &texCharArray2);

		glGenTextures(1, &Mercury);
		glBindTexture(GL_TEXTURE_2D, Mercury);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW2, texH2, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray2);

		free(texCharArray2);
		free(texarray2);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	{
		RGBTRIPLE* texarray3;
		char* texCharArray3;
		int texW3, texH3;
		OpenGL::LoadBMP("Venus.bmp", &texW3, &texH3, &texarray3);
		OpenGL::RGBtoChar(texarray3, texW3, texH3, &texCharArray3);

		glGenTextures(1, &Venus);
		glBindTexture(GL_TEXTURE_2D, Venus);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW3, texH3, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray3);

		free(texCharArray3);
		free(texarray3);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	{
		RGBTRIPLE* texarray4;
		char* texCharArray4;
		int texW4, texH4;
		OpenGL::LoadBMP("Sun.bmp", &texW4, &texH4, &texarray4);
		OpenGL::RGBtoChar(texarray4, texW4, texH4, &texCharArray4);

		glGenTextures(1, &Sun);
		glBindTexture(GL_TEXTURE_2D, Sun);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW4, texH4, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray4);

		free(texCharArray4);
		free(texarray4);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	{
		RGBTRIPLE* texarray5;
		char* texCharArray5;
		int texW5, texH5;
		OpenGL::LoadBMP("Mars.bmp", &texW5, &texH5, &texarray5);
		OpenGL::RGBtoChar(texarray5, texW5, texH5, &texCharArray5);

		glGenTextures(1, &Mars);
		glBindTexture(GL_TEXTURE_2D, Mars);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW5, texH5, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray5);

		free(texCharArray5);
		free(texarray5);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	{
		RGBTRIPLE* texarray6;
		char* texCharArray6;
		int texW6, texH6;
		OpenGL::LoadBMP("Jupiter.bmp", &texW6, &texH6, &texarray6);
		OpenGL::RGBtoChar(texarray6, texW6, texH6, &texCharArray6);

		glGenTextures(1, &Jupiter);
		glBindTexture(GL_TEXTURE_2D, Jupiter);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW6, texH6, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray6);

		free(texCharArray6);
		free(texarray6);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	{
		RGBTRIPLE* texarray7;
		char* texCharArray7;
		int texW7, texH7;
		OpenGL::LoadBMP("Saturn.bmp", &texW7, &texH7, &texarray7);
		OpenGL::RGBtoChar(texarray7, texW7, texH7, &texCharArray7);

		glGenTextures(1, &Saturn);
		glBindTexture(GL_TEXTURE_2D, Saturn);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW7, texH7, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray7);

		free(texCharArray7);
		free(texarray7);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	{
		RGBTRIPLE* texarray8;
		char* texCharArray8;
		int texW8, texH8;
		OpenGL::LoadBMP("Uranus.bmp", &texW8, &texH8, &texarray8);
		OpenGL::RGBtoChar(texarray8, texW8, texH8, &texCharArray8);

		glGenTextures(1, &Uranus);
		glBindTexture(GL_TEXTURE_2D, Uranus);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW8, texH8, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray8);

		free(texCharArray8);
		free(texarray8);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	{
		RGBTRIPLE* texarray9;
		char* texCharArray9;
		int texW9, texH9;
		OpenGL::LoadBMP("Neptune.bmp", &texW9, &texH9, &texarray9);
		OpenGL::RGBtoChar(texarray9, texW9, texH9, &texCharArray9);

		glGenTextures(1, &Neptune);
		glBindTexture(GL_TEXTURE_2D, Neptune);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW9, texH9, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray9);

		free(texCharArray9);
		free(texarray9);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	{
		RGBTRIPLE* texarray10;
		char* texCharArray10;
		int texW10, texH10;
		OpenGL::LoadBMP("Moon.bmp", &texW10, &texH10, &texarray10);
		OpenGL::RGBtoChar(texarray10, texW10, texH10, &texCharArray10);

		glGenTextures(1, &Moon);
		glBindTexture(GL_TEXTURE_2D, Moon);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW10, texH10, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray10);

		free(texCharArray10);
		free(texarray10);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	{
		RGBTRIPLE* texarray12;
		char* texCharArray12;
		int texW12, texH12;
		OpenGL::LoadBMP("SaturnRing.bmp", &texW12, &texH12, &texarray12);
		OpenGL::RGBtoChar(texarray12, texW12, texH12, &texCharArray12);

		glGenTextures(1, &SaturnRing);
		glBindTexture(GL_TEXTURE_2D, SaturnRing);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW12, texH12, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray12);

		free(texCharArray12);
		free(texarray12);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	{
		RGBTRIPLE* texarray13;
		char* texCharArray13;
		int texW13, texH13;
		OpenGL::LoadBMP("UranRing.bmp", &texW13, &texH13, &texarray13);
		OpenGL::RGBtoChar(texarray13, texW13, texH13, &texCharArray13);

		glGenTextures(1, &UranusRing);
		glBindTexture(GL_TEXTURE_2D, UranusRing);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW13, texH13, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray13);

		free(texCharArray13);
		free(texarray13);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}

void drawPlanet(float radius, int stacks, int slices, float startX, float startY, float startZ, bool Sun) {
	for (int i = 0; i <= stacks; ++i) {
		float theta1 = i * M_PI / stacks - M_PI / 2.0f;
		float theta2 = (i + 1) * M_PI / stacks - M_PI / 2.0f;

		glBegin(GL_TRIANGLE_STRIP);
		for (int j = 0; j <= slices; ++j) {
			float phi = j * 2 * M_PI / slices;

			float x1 = cos(phi) * cos(theta1) * radius + startX;
			float y1 = sin(phi) * cos(theta1) * radius + startY;
			float z1 = sin(theta1) * radius + startZ;
			float u1 = (float)j / slices;
			float v1 = 1.0f - (float)i / stacks;
			glTexCoord2f(u1, v1);
			if (Sun)
				glNormal3f(-(x1 - startX), -(y1 - startY), -(z1 - startZ));
			else
				glNormal3f((x1 - startX), (y1 - startY), (z1 - startZ));
			glVertex3f(x1, y1, z1);

			float x2 = cos(phi) * cos(theta2) * radius + startX;
			float y2 = sin(phi) * cos(theta2) * radius + startY;
			float z2 = sin(theta2) * radius + startZ;
			float u2 = (float)j / slices;
			float v2 = 1.0f - (float)(i + 1) / stacks;
			glTexCoord2f(u2, v2);
			if (Sun)
				glNormal3f(-(x2 - startX), -(y2 - startY), -(z2 - startZ));
			else
				glNormal3f((x2 - startX), (y2 - startY), (z2 - startZ));
			glVertex3f(x2, y2, z2);
		}
		glEnd();
	}
}

void drawStars(float stars[][3], int numStars) {
	glPointSize(1.2f);

	glBegin(GL_POINTS);
	for (int i = 0; i < numStars; ++i) {
		float x = stars[i][0];
		float y = stars[i][1];
		float z = stars[i][2];

		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(x, y, z);
	}
	glEnd();
}

void generateStars(float stars[][3], int numStars, float sphereRadius) {
	for (int i = 0; i < numStars; ++i) {
		float theta = ((float)rand() / RAND_MAX) * 2 * 3.14159f;
		float phi = acos(1 - 2 * ((float)rand() / RAND_MAX));

		stars[i][0] = sphereRadius * sin(phi) * cos(theta);
		stars[i][1] = sphereRadius * sin(phi) * sin(theta);
		stars[i][2] = sphereRadius * cos(phi);
	}
}

void drawRing(GLfloat innerRadius, GLfloat outerRadius, GLfloat offsetX, GLfloat offsetY, GLfloat offsetZ) 
{
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= 50; ++i) {
		GLfloat theta = 2.0f * M_PI * float(i) / float(50);
		GLfloat x = cos(theta);
		GLfloat y = sin(theta);

		glNormal3f(0.0f, 1.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(innerRadius * x + offsetX, innerRadius * y + offsetY, offsetZ);
		glVertex3f(outerRadius * x + offsetX, outerRadius * y + offsetY, offsetZ);
	}
	glEnd();
}

void draworb(float radius, int segments) 
{
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < segments; ++i) {
		float theta = 2.0f * M_PI * float(i) / float(segments);
		float x = radius * cosf(theta);
		float y = radius * sinf(theta);
		glVertex2f(x, y);
	}
	glEnd();
}

bool generated = false;
const int numStars = 1000;
float stars[numStars][3];
int k = 10; 
double h = 1;

float rotationAngle = 0.0f;

void Render(OpenGL* ogl)
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);

	GLfloat amb[] = { 0.2, 0.2, 0.2, 1. };
	GLfloat dif[] = { 1.0, 1.0, 1.0, 1. };
	GLfloat spec[] = { 1.0, 1.0, 1.0, 1. };
	GLfloat sh = 0.1f * 256;

	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут 

	if (h < 50 && h > 0 || h < -1) 
		if (OpenGL::isKeyPressed('F')) { h = h + 1; }
	if (h > 1 || h > -50 && h < 0)
		if (OpenGL::isKeyPressed('R')) { h = h - 1; }
	if (k > 10) 
		if (OpenGL::isKeyPressed('N')) { k = k - 1; }
	if (k < 100) 
		if (OpenGL::isKeyPressed('M')) { k = k + 1; }

	float SunSize = 30.0f / k;
	float MercurySize = 6.1f / k;
	float VenusSize = 8.75f / k;
	float EarthSize = 10.2f / k;
	float MarsSize = 12.27f / k;
	float JupiterSize = 14.2f / k;
	float SaturnSize = 16.3f / k;
	float UranusSize = 18.9f / k;
	float NeptuneSize = 20.8f / k;
	float MoonSize = 3.14f / k;

	float MercuryorbRadius = 60.0f / k;
	float VenusorbRadius = 120.0f / k;
	float EarthorbRadius = 180.0f / k;
	float MarsorbRadius = 240.0f / k;
	float JupiterorbRadius = 300.0f / k;
	float SaturnorbRadius = 400.0f / k;
	float UranusorbRadius = 500.0f / k;
	float NeptuneorbRadius = 600.0f / k;

	float MercuryorbSpeed = 0.0159f * h;
	float VenusorbSpeed = 0.0117f * h;
	float EarthorbSpeed = 0.01f * h;
	float MarsorbSpeed = 0.00811f * h;
	float JupiterorbSpeed = 0.00439f * h;
	float SaturnorbSpeed = 0.00325f * h;
	float UranusorbSpeed = 0.00228f * h;
	float NeptuneorbSpeed = 0.00182f * h;
	float MoonorbSpeed = 0.000343f * h;

	if (real)
	{
		 SunSize = 50.0f / k;
		 MercurySize = 1.91f / k;
		 VenusSize = 4.75f / k;
		 EarthSize = 10.0f / k;
		 MarsSize = 5.32f / k;
		 JupiterSize = 55.2f / k;
		 SaturnSize = 40.3f / k;
		 UranusSize = 21.3f / k;
		 NeptuneSize = 38.6f / k;
		 MoonSize = 2.73f / k;

		 MercuryorbRadius = 144.6f / k;
		 VenusorbRadius = 289.2f / k;
		 EarthorbRadius = 400.0f / k;
		 MarsorbRadius = 604.8f / k;
		 JupiterorbRadius = 1240.8f / k;
		 SaturnorbRadius = 1920.0f / k;
		 UranusorbRadius = 3838.0f / k;
		 NeptuneorbRadius = 6020.0f / k;
	}

	if (orb)
	{
		glDisable(GL_LIGHTING);
		draworb(MercuryorbRadius, 100);
		draworb(VenusorbRadius, 100);
		draworb(EarthorbRadius, 100);
		draworb(MarsorbRadius, 100);
		draworb(JupiterorbRadius, 100);
		draworb(SaturnorbRadius, 100);
		draworb(UranusorbRadius, 100);
		draworb(NeptuneorbRadius, 100);
		glEnable(GL_LIGHTING);
	}

	if (st) 
	{
		MercuryorbSpeed = 0.0f * h;
		VenusorbSpeed = 0.0f * h;
		EarthorbSpeed = 0.0f * h;
		MarsorbSpeed = 0.0f * h;
		JupiterorbSpeed = 0.0f * h;
		SaturnorbSpeed = 0.0f * h;
		UranusorbSpeed = 0.0f * h;
		NeptuneorbSpeed = 0.0f * h;
	}

	if (rotation)
	{
		h = -h;
		rotation = !rotation;
	}
	
	static float Mercury_angle = 0.0f;
	static float Venus_angle = 0.0f;
	static float Earth_angle = 0.0f;
	static float Mars_angle = 0.0f;
	static float Jupiter_angle = 0.0f;
	static float Saturn_angle = 0.0f;
	static float Uranus_angle = 0.0f;
	static float Neptune_angle = 0.0f;
	static float Moon_angle = 0.0f;

	float Universal_z = 0.0;

	float Mercury_x = MercuryorbRadius * cos(Mercury_angle);
	float Mercury_y = MercuryorbRadius * sin(Mercury_angle);

	float Venus_x = VenusorbRadius * cos(Venus_angle);
	float Venus_y = VenusorbRadius * sin(Venus_angle);

	float Earth_x = EarthorbRadius * cos(Earth_angle);
	float Earth_y = EarthorbRadius * sin(Earth_angle);

	float Mars_x = MarsorbRadius * cos(Mars_angle);
	float Mars_y = MarsorbRadius * sin(Mars_angle);

	float Jupiter_x = JupiterorbRadius * cos(Jupiter_angle);
	float Jupiter_y = JupiterorbRadius * sin(Jupiter_angle);

	float Saturn_x = SaturnorbRadius * cos(Saturn_angle);
	float Saturn_y = SaturnorbRadius * sin(Saturn_angle);

	float Uranus_x = UranusorbRadius * cos(Uranus_angle);
	float Uranus_y = UranusorbRadius * sin(Uranus_angle);

	float Neptune_x = NeptuneorbRadius * cos(Neptune_angle);
	float Neptune_y = NeptuneorbRadius * sin(Neptune_angle);

	float Saturn_Ring_Inner_Radius = 20 / k;
	float Saturn_Ring_Outer_Radius = 40 / k;

	float Uranus_Ring_Inner_Radius = 10 / k;
	float Uranus_Ring_Outer_Radius = 20 / k;

	float Saturn_Ring_X = SaturnorbRadius * cos(Saturn_angle);
	float Saturn_Ring_Y = SaturnorbRadius * sin(Saturn_angle);

	float Uranus_Ring_X = UranusorbRadius * cos(Uranus_angle);
	float Uranus_Ring_Y = UranusorbRadius * sin(Uranus_angle);

	static float SunAngle = 0.0f;
	static float MercuryAngle = 0.0f;
	static float VenusAngle = 0.0f;
	static float EarthAngle = 0.0f;
	static float MarsAngle = 0.0f;
	static float JupiterAngle = 0.0f;
	static float SaturnAngle = 0.0f;
	static float UranusAngle = 0.0f;
	static float NeptuneAngle = 0.0f;
	static float MoonAngle = 0.0f;

	float SunorbSpeedOwnStep = 0.3f * h;
	float MercuryorbSpeedOwnStep = 3.0f * h;
	float VenusorbSpeedOwnStep = 3.0f * h;
	float EarthorbSpeedOwnStep = 3.0f * h;
	float MarsorbSpeedOwnStep = 3.0f * h;
	float JupiterorbSpeedOwnStep = 2.0f * h;
	float SaturnorbSpeedOwnStep = 2.0f * h;
	float UranusorbSpeedOwnStep = 2.0f * h;
	float NeptuneorbSpeedOwnStep = 2.0f * h;
	float MoonorbSpeedOwnStep = 6.0f * h;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glDisable(GL_LIGHTING);
	if (generated == false) 
		generateStars(stars, numStars, 300.0f);
	drawStars(stars, numStars);
	generated = true;
	glEnable(GL_LIGHTING);

	//Солнце
	glBindTexture(GL_TEXTURE_2D, Sun);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(SunAngle, 0.0f, 0.0f, 1.0f);
	drawPlanet(SunSize,20, 200, 0, 0, 0, true);
	glPopMatrix();

	//Меркурий
	glBindTexture(GL_TEXTURE_2D, Mercury);
	glPushMatrix();
	glTranslatef(Mercury_x, Mercury_y, Universal_z);
	glRotatef(MercuryAngle, 0.0f, 0.0f, 1.0f);
	drawPlanet(MercurySize, 20, 20, 0, 0, 0, false);
	glPopMatrix();	
	Mercury_angle += MercuryorbSpeed;

	//Венера
	glBindTexture(GL_TEXTURE_2D, Venus);
	glPushMatrix();
	glTranslatef(Venus_x, Venus_y, Universal_z);
	glRotatef(VenusAngle, 0.0f, 0.0f, 1.0f);
	drawPlanet(VenusSize, 20, 20, 0, 0, 0, false);
	glPopMatrix();
	Venus_angle += VenusorbSpeed;

	//Земля
	glBindTexture(GL_TEXTURE_2D, Earth);
	glPushMatrix();
	glTranslatef(Earth_x, Earth_y, Universal_z);
	glRotatef(EarthAngle, 0.0f, 0.0f, 1.0f);
	drawPlanet(EarthSize, 20, 20, 0, 0, 0, false);
	glPopMatrix();
	Earth_angle += EarthorbSpeed;

	//Луна
	float MoonDistanceCoefficient = 1.15f;
	glBindTexture(GL_TEXTURE_2D, Moon);
	glPushMatrix();
	glTranslatef(Earth_x, Earth_y, Universal_z);
	glRotatef(MoonAngle, 0.0f, 0.0f, 1.0f);
	drawPlanet(MoonSize, 20, 20, Earth_x-Earth_x/MoonDistanceCoefficient, Earth_y-Earth_y/MoonDistanceCoefficient, Universal_z, false);
	glPopMatrix();
	
	//Марс
	glBindTexture(GL_TEXTURE_2D, Mars);
	glPushMatrix();
	glTranslatef(Mars_x, Mars_y, Universal_z);
	glRotatef(EarthAngle, 0.0f, 0.0f, 1.0f);
	drawPlanet(MarsSize, 20, 20, 0, 0, 0, false);
	glPopMatrix();
	Mars_angle += MarsorbSpeed;

	//Юпитер
	glBindTexture(GL_TEXTURE_2D, Jupiter);
	glPushMatrix();
	glTranslatef(Jupiter_x, Jupiter_y, Universal_z);
	glRotatef(JupiterAngle, 0.0f, 0.0f, 1.0f);
	drawPlanet(JupiterSize, 20, 20, 0, 0, 0, false);
	glPopMatrix();
	Jupiter_angle += JupiterorbSpeed;

	//Сатурн
	glBindTexture(GL_TEXTURE_2D, Saturn);
	glPushMatrix();
	glTranslatef(Saturn_x, Saturn_y, Universal_z);
	glRotatef(SaturnAngle, 0.0f, 0.0f, 1.0f);
	drawPlanet(SaturnSize, 20, 20, 0, 0, 0, false);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, SaturnRing);
	drawRing(SaturnSize + Saturn_Ring_Inner_Radius, SaturnSize + Saturn_Ring_Outer_Radius, Saturn_Ring_X, Saturn_Ring_Y, 0);
	Saturn_angle += SaturnorbSpeed;

	//Уран
	glBindTexture(GL_TEXTURE_2D, Uranus);
	glPushMatrix();
	glTranslatef(Uranus_x, Uranus_y, Universal_z);
	glRotatef(UranusAngle, 0.0f, 0.0f, 1.0f);
	drawPlanet(UranusSize, 20, 20, 0, 0, 0, false);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, UranusRing);
	drawRing(UranusSize + Uranus_Ring_Inner_Radius, UranusSize + Uranus_Ring_Outer_Radius, Uranus_Ring_X, Uranus_Ring_Y, 0);
	Uranus_angle += UranusorbSpeed;

	//Нептун
	glBindTexture(GL_TEXTURE_2D, Neptune);
	glPushMatrix();
	glTranslatef(Neptune_x, Neptune_y, Universal_z);
	glRotatef(NeptuneAngle, 0.0f, 0.0f, 1.0f);
	drawPlanet(NeptuneSize, 20, 20, 0, 0, 0, false);
	glPopMatrix();
	Neptune_angle += NeptuneorbSpeed;

	if (!st)
	{
		SunAngle = SunAngle + SunorbSpeedOwnStep;
		MercuryAngle = MercuryAngle + MercuryorbSpeedOwnStep;
		VenusAngle = VenusAngle + VenusorbSpeedOwnStep;
		EarthAngle = EarthAngle + EarthorbSpeedOwnStep;
		MoonAngle = MoonAngle + MoonorbSpeedOwnStep;
		MarsAngle = MarsAngle + MarsorbSpeedOwnStep;
		JupiterAngle = JupiterAngle + JupiterorbSpeedOwnStep;
		SaturnAngle = SaturnAngle + SaturnorbSpeedOwnStep;
		UranusAngle = UranusAngle + UranusorbSpeedOwnStep;
		NeptuneAngle = NeptuneAngle + NeptuneorbSpeedOwnStep;
	}

	//Сообщение вверху экрана
	glMatrixMode(GL_PROJECTION);	//Делаем активной матрицу проекций. 
	//(всек матричные операции, будут ее видоизменять.)
	glPushMatrix();   //сохраняем текущую матрицу проецирования (которая описывает перспективную проекцию) в стек 				    
	glLoadIdentity();	  //Загружаем единичную матрицу
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //врубаем режим ортогональной проекции
	glMatrixMode(GL_MODELVIEW);		//переключаемся на модел-вью матрицу
	glPushMatrix();			  //сохраняем текущую матрицу в стек (положение камеры, фактически)
	glLoadIdentity();		  //сбрасываем ее в дефолт
	glDisable(GL_LIGHTING);

	GuiTextRectangle rec;		   //классик моего авторства для удобной работы с рендером текста.
	rec.setSize(500, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);

	std::stringstream ss;
	ss << "T - Вкл/Выкл линии орбит" << std::endl;
	ss << "S - Запустить/остановить орбиту" << std::endl;
	ss << "F/R - Ускорение/Замедление" << std::endl;
	ss << "U - Изменить направление вращения" << std::endl;
	ss << "G - Переключение на реальный масштаб " << std::endl;
	ss << "N/M - Приближение/Отдаление " << std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();
	glMatrixMode(GL_PROJECTION);	  //восстанавливаем матрицы проекции и модел-вью обратно из стека.
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}