﻿#include <stdio.h>
#include <Windows.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include <Eigen/Dense>
#include <Eigen/Geometry>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define KEY_ESC 27 /* GLUT doesn't supply this */
#define KEY_SPACEBAR 32 /* GLUT doesn't supply this */
#define M_PI 3.14159265358979323846
#define DEGS_TO_RADS 0.01745329252
#define RADS_TO_DEGS 57.295779513


using namespace std;
using namespace Eigen;

bool fullscreen = false;
bool mouseDown = false;
bool idleRotation = false;

float xrot = 0.0f;
float yrot = 0.0f;
float zrot = 0.0f;

float xdiff = 0.0f;
float ydiff = 0.0f;
float zdiff = 0.0f;

float tra_x = 0.0f;
float tra_y = 0.0f;
float tra_z = 0.0f;

float grow_shrink = 60.0f;
float resize_f = 1.0f;

Matrix3d EulerAnglesToRotationMatrix(float yaw, float pitch, float roll)
{
	Matrix3d R;
	R << cos(pitch) * cos(yaw),
		cos(yaw)* sin(pitch)* sin(roll) - cos(roll) * sin(yaw),
		cos(roll)* cos(yaw)* sin(pitch) + sin(yaw) * sin(roll),
		cos(pitch)* sin(yaw),
		sin(yaw)* sin(pitch)* sin(roll) + cos(roll) * cos(yaw),
		sin(pitch)* sin(yaw)* cos(roll) - cos(yaw) * sin(roll),
		-sin(pitch),
		cos(pitch)* sin(roll),
		cos(pitch)* cos(roll);

	return R;
}
Vector4d rotationMatrixToEulerAxisAngle(Matrix3d rotMat)
{
	Vector3d axis;
	float angle;

	angle = acos((rotMat.trace() - 1) / 2);

	Matrix3d Ux = ((rotMat - rotMat.transpose()) / 2 * sin(angle));
	axis.x() = Ux(2, 1);
	axis.y() = Ux(0, 2);
	axis.z() = Ux(1, 0);

	angle *= RADS_TO_DEGS;

	return { axis.x(), axis.y(), axis.z(), angle };
}
Vector3d EulerAxisAngleToRotationVector(Vector3d axis, float angle)
{
	axis.normalize();

	Vector3d vec;
	vec.x() = axis.x() * angle;
	vec.y() = axis.y() * angle;
	vec.z() = axis.z() * angle;

	return vec;
}
Quaternionf EulerAxisAngleToQuaternion(Vector3d axis, float angle)
{
	Quaternionf q;

	q.w() = cos(angle / 2);
	q.x() = axis.x() * sin(angle / 2);
	q.y() = axis.y() * sin(angle / 2);
	q.z() = axis.z() * sin(angle / 2);

	return q;
}

void resetScenario()
{
	xrot = 0.0f;
	yrot = 0.0f;
	zrot = 0.0f;

	xdiff = 100.0f;
	ydiff = 100.0f;
	zdiff = 100.0f;
	 
	tra_x = 0.0f;
	tra_y = 0.0f;
	tra_z = 0.0f;
}

void drawCube()
{
	glTranslatef(tra_x, tra_y, tra_z);

	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.0f, 0.0f);
	// FRONT
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, 0.5f, 0.5f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-0.5f, 0.5f, 0.5f);
	// BACK
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.5f, 0.5f, -0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	// LEFT
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(-0.5f, 0.5f, 0.5f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	// RIGHT
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(0.5f, 0.5f, -0.5f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.5f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	glColor3f(0.0f, 0.0f, 1.0f);
	// TOP
	glVertex3f(-0.5f, 0.5f, 0.5f);
	glVertex3f(0.5f, 0.5f, 0.5f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.5f, 0.5f, -0.5f);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glColor3f(1.0f, 0.0f, 0.0f);
	// BOTTOM
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glEnd();
}

void drawLine(GLfloat* lineVertices, int numVertices,int lineWidth, int dimensions, float color_r, float color_g, float color_b)
{
	glColor3f(color_r, color_g, color_b);

	glLineWidth(lineWidth);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(dimensions, GL_FLOAT, 0, lineVertices);
	glDrawArrays(GL_LINES, 0, numVertices);
	glDisableClientState(GL_VERTEX_ARRAY);

}
void renderbitmap(float x, float y, void* font, char* string)
{
	char* c;
	glRasterPos2f(x, y);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void blitText()
{
	glColor3d(0.0f, 0.0f, 0.0f);

	char title[35];
	sprintf_s(title, "Maths 2 Project - Adria Pons Mensa");
	renderbitmap(2.68f, 1.1f, GLUT_BITMAP_HELVETICA_12, title);

	char subtitle[30];
	sprintf_s(subtitle, "ATTITUDE  REPRESENTATION");
	renderbitmap(2.35f, 1.0f, GLUT_BITMAP_TIMES_ROMAN_24, subtitle);


	// Quaternion
	glColor3d(1.0f, .0f, .0f);
	char quaternion[30];
	sprintf_s(quaternion, "Quaternion:");
	renderbitmap(2.555f, 0.7f, GLUT_BITMAP_HELVETICA_18, quaternion);

	// Euler Axis/Angle
	glColor3d(.0f, .75f, .0f);
	char eulerAngleAxis[32];
	sprintf_s(eulerAngleAxis, "Euler principal Angle and Axis:");
	renderbitmap(2.555f, 0.2f, GLUT_BITMAP_HELVETICA_18, eulerAngleAxis);

	// Euler Angles
	glColor3d(.0f, .0f, 1.0f);
	char eulerAngles[14];
	sprintf_s(eulerAngles, "Euler angles:");
	renderbitmap(2.555f, -0.3f, GLUT_BITMAP_HELVETICA_18, eulerAngles);
	char angles[50];
	sprintf_s(angles, "Yaw: %.2f   Pitch: %.2f   Roll: %.2f", xrot, yrot, zrot);
	renderbitmap(2.555f, -0.45f, GLUT_BITMAP_HELVETICA_18, angles);


	// Rotation vector
	glColor3d(1.0f, .0f, 1.0f);
	char rotVec[17];
	sprintf_s(rotVec, "Rotation vector:");
	renderbitmap(2.555f, -0.8f, GLUT_BITMAP_HELVETICA_18, rotVec);

	// Rotation matrix
	glColor3d(.5f, .5f, .5f);
	char rotMat[17];
	sprintf_s(rotMat, "Rotation matrix:");
	renderbitmap(2.555f, -1.3f, GLUT_BITMAP_HELVETICA_18, rotMat);
	

	Matrix3d R;
	R = EulerAnglesToRotationMatrix(xrot, yrot, zrot);
	char row1[30];
	sprintf_s(row1, "%.2f     %.2f     %.2f", R(0, 0), R(0, 1), R(0, 2));
	renderbitmap(2.555f, -1.5f, GLUT_BITMAP_HELVETICA_18, row1);
	char row2[30];
	sprintf_s(row2, "%.2f     %.2f     %.2f", R(1, 0), R(1, 1), R(1, 2));
	renderbitmap(2.555f, -1.6f, GLUT_BITMAP_HELVETICA_18, row2);
	char row3[30];
	sprintf_s(row3, "%.2f     %.2f     %.2f", R(2, 0), R(2, 1), R(2, 2));
	renderbitmap(2.555f, -1.7f, GLUT_BITMAP_HELVETICA_18, row3);

	Vector4d vec;
	vec = rotationMatrixToEulerAxisAngle(R);
	glColor3d(.0f, .75f, .0f);
	char angle[20];
	sprintf_s(angle, "%.2f degs", vec.w());
	renderbitmap(2.555f, 0.05, GLUT_BITMAP_HELVETICA_18, angle);
	char axis[30];
	sprintf_s(axis, "(%.2f,  %.2f,  %.2f)", vec.x(), vec.y(), vec.z());
	renderbitmap(2.555f, -0.05, GLUT_BITMAP_HELVETICA_18, axis);

	Quaternionf q;
	q = EulerAxisAngleToQuaternion({ vec.x(), vec.y(), vec.z() }, vec.w() * RADS_TO_DEGS);
	glColor3d(1.0f, .0f, .0f);
	char quaternionRes[40];
	sprintf_s(quaternionRes, "(%.2f,  %.2fi,  %.2fj, %.2fk)", q.w(), q.x(), q.y(), q.z());
	renderbitmap(2.555f, 0.55, GLUT_BITMAP_HELVETICA_18, quaternionRes);

	Vector3d rotationVec;
	rotationVec = EulerAxisAngleToRotationVector({ vec.x(), vec.y(), vec.z() }, vec.w() * DEGS_TO_RADS);
	glColor3d(1.0f, .0f, 1.0f);
	char rotationVector[30];
	sprintf_s(rotationVector, "(%.2f,  %.2f,  %.2f)", rotationVec.x(), rotationVec.y(), rotationVec.z());
	renderbitmap(2.555f, -0.95, GLUT_BITMAP_HELVETICA_18, rotationVector);

}


int init()
{
	glClearColor(0.9f, 0.9f, 0.9f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);

	return 1;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	

	gluLookAt(0.15, 0.5, 3.0,
			  0.0, 0.0, 0.0,
			  0.17, 1.0, 0.0);
	
	// Called before glRotatef(), so they are NOT rotated
	blitText();

	GLfloat lineVertices[] =
	{
		2,2,0,
		1.715,-2.5,0
	};
	drawLine(lineVertices, 2, 10, 3, 0, 0, 0);

	
	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	if (xrot > 360 || xrot < -360)
		xrot = 0;
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
	if (yrot > 360 || yrot < -360)
		yrot = 0;
	glRotatef(zrot, 0.0f, 0.0f, 1.0f);
	if (zrot > 360 || zrot < -360)
		zrot = 0;

	// Called after glRotatef(), everything after the function will be rotated
	drawCube();
	

	glFlush();
	glutSwapBuffers();
}

void resize(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(-300, 0, w, h);

	gluPerspective(grow_shrink, resize_f * w / h, resize_f, 100 * resize_f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// USER INPUT
void keyboardInput(unsigned char key, int x, int y)
{
	if (!idleRotation)
	{
		switch (key)
		{
		case 'w':
		case 'W':
			xrot -= 1.0f;
			xdiff -= 1.0f;
			break;

		case 's':
		case 'S':
			xrot += 1.0f;
			xdiff += 1.0f;
			break;

		case 'a':
		case 'A':
			yrot += 1.0f;
			ydiff += 1.0f;
			break;

		case 'd':
		case 'D':
			yrot -= 1.0f;
			ydiff -= 1.0f;
			break;

		case 'q':
		case 'Q':
			zrot += 1.0f;
			zdiff += 1.0f;
			break;

		case 'e':
		case 'E':
			zrot -= 1.0f;
			zdiff -= 1.0f;
			break;
		}
	}

	switch (key)
	{
	case KEY_SPACEBAR:
		idleRotation = !idleRotation;
		break;

	case 'r':
	case 'R':
		resetScenario();
		break;

	case KEY_ESC:		// ESC key
		exit(1);
		break;
	}

	glutPostRedisplay();
}
void specialKeyboardInput(int key, int x, int y)
{
	// Enable/Disable FULLSCREEN MODE
	if (key == GLUT_KEY_F11)
	{
		fullscreen = !fullscreen;

		if (fullscreen)
			glutFullScreen();
		else
		{
			glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
			glutPositionWindow(0, 0);
		}
	}

	// Translate Cube
	if (key == GLUT_KEY_RIGHT)
	{
		tra_z -= 0.01f;
	}
	if (key == GLUT_KEY_LEFT)
	{
		tra_z += 0.01f;
	}
	if (key == GLUT_KEY_UP) 
	{
		tra_x -= 0.01f;
	}
	if (key == GLUT_KEY_DOWN)
	{
		tra_x += 0.01f;
	}   
	
	glutPostRedisplay();
}
void mouseInput(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		mouseDown = 1;

		xdiff = x - yrot;
		
		ydiff = -y + xrot;
		
	}
	else
		mouseDown = 0;
}

// Mouse cube rotation
void mouseMotionRotation(int x, int y)
{
	if (mouseDown)
	{
		yrot = x - xdiff;
		if (yrot > 360 || yrot < -360)
			yrot = 0;
		xrot = y + ydiff;
		if (xrot > 360 || xrot < -360)
			xrot = 0;

		glutPostRedisplay();
	}
}
void idleCubeRotation(void)
{
	if (idleRotation)
	{
		if (!mouseDown)
		{
			xrot += 0.1f;
			yrot += 0.1f;
			zrot += 0.1f;
		}
	}

	glutPostRedisplay();
}

void infoConsoleDisplay()
{
	cout << "QWEASD ------> Rotate Cube" << endl;
	cout << "ARROWS ------> Translate Cube" << endl;
	cout << "SPACEBAR ------> Idle Cube Rotation" << endl;
	cout << "R ------> Reset Scenario" << endl;
	cout << "F11 ------> Fullscreen" << endl;
	cout << "ESCAPE ------> EXIT" << endl << endl;
}

int main(int argc, char* argv[])
{
	infoConsoleDisplay();

	glutInit(&argc, argv);

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	glutCreateWindow("Maths 2 Project | Adria Pons Mensa | Universitat Politècnica de Catalunya · BarcelonaTech");
	
	glutDisplayFunc(display);
	
	// Keyboard input
	glutKeyboardFunc(keyboardInput);
	// Special keys input
	glutSpecialFunc(specialKeyboardInput);
	// Mouse input
	glutMouseFunc(mouseInput);

	glutMotionFunc(mouseMotionRotation);
	glutReshapeFunc(resize);
	glutIdleFunc(idleCubeRotation);
	

	if (!init())
		return 1;

	glutMainLoop();

	return 0;
}