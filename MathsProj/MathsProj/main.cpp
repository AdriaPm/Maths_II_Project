#include <stdio.h>
#include <Windows.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <iostream>
#include <math.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define KEY_ESC 27 /* GLUT doesn't supply this */

using namespace std;

bool fullscreen = false;
bool mouseDown = false;
bool idleRotation = false;

float xrot = 100.0f;
float yrot = -100.0f;

float xdiff = 100.0f;
float ydiff = 100.0f;

float tra_x = 0.0f;
float tra_y = 0.0f;
float tra_z = 0.0f;

float grow_shrink = 60.0f;
float resize_f = 1.0f;

void resetScenario()
{
	xrot = 100.0f;
	yrot = -100.0f;

	xdiff = 100.0f;
	ydiff = 100.0f;

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

void drawRect()
{
	glColor3f(0, 0, 0);
	glRectf(0.0f, 0.0f, 1.0f, 1.0f);



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
	glColor3d(1.0f, 0.0f, 0.0f);
	char text[100] = { 0 };
	sprintf_s(text, "Hello World");
	renderbitmap(3.0f, 1.0f, GLUT_BITMAP_TIMES_ROMAN_24, text);
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
	drawRect();


	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
	
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

		case 'Z':
		case 'z':
			grow_shrink--;
			resize(WINDOW_WIDTH, WINDOW_HEIGHT);
			break;

		case 'X':
		case 'x':
			grow_shrink++;
			resize(WINDOW_WIDTH, WINDOW_HEIGHT);
			break;
		}
	}

	switch (key)
	{
	case 'f':
	case 'F':
		idleRotation = !idleRotation;
		break;

	case 'r':
	case 'R':
		resetScenario();
		break;

	case 27:		// ESC key
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
			glutPositionWindow(200, 200);
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
		xrot = y + ydiff;

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
		}
	}

	glutPostRedisplay();
}

void infoConsoleDisplay()
{
	cout << "WASD ------> Rotate Cube" << endl;
	cout << "ARROWS ------> Translate Cube" << endl;
	cout << "X ------> Shrink" << endl;
	cout << "Z ------> Grow" << endl;
	cout << "F ------> Idle Cube Rotation" << endl;
	cout << "R ------> Reset Scenario" << endl;
	cout << "F11 ------> Fullscreen" << endl;
	cout << "ESCAPE ------> EXIT" << endl << endl;
}

int main(int argc, char* argv[])
{
	infoConsoleDisplay();

	glutInit(&argc, argv);

	glutInitWindowPosition(200, 200);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

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