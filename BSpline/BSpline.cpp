﻿#include <iostream>
#include "glut.h"
#include "bspline.h"

bool mouseLeftDown, mouseRightDown;
float cameraAngleX = 0.0, cameraAngleY = 0.0;
float mouseX, mouseY;
float cameraDistance = -20;
extern void displayB();
void mouseCB(int button, int state, int x, int y) {
	mouseX = x, mouseY = y;
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			mouseLeftDown = true;
		}
		else {
			mouseLeftDown = false;
		}
	}
	else if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			mouseRightDown = true;
		}
		else {
			mouseRightDown = false;
		}
	}
}
void mouseMotionCB(int x, int y) {
	fprintf(stderr, "%d %d", x, y);
	if (mouseLeftDown) {
		cameraAngleX += (x - mouseX);
		cameraAngleY += (y - mouseY);
		mouseX = x;
		mouseY = y;
	}
	else {
		cameraDistance -= (y - mouseY) * 0.2f;
		mouseY = y;
	}
	glutPostRedisplay();
}

void display(void) {
	
	glClear(GL_COLOR_BUFFER_BIT);
	// 绘制世界坐标系
	
	// 在原点处，建立一个四面体，四个顶点分别位于三个轴上，和原点处。
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	/*
	glBegin(GL_TRIANGLES);
	glColor3f(1, 0, 0);
	glVertex3d(0, 1, 0);
	glVertex3d(1, 0, 0);
	glVertex3d(0, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3d(1, 0, 0);
	glVertex3d(0, 0, 2);
	glVertex3d(0, 0, 0);

	glColor3f(0, 0, 1);
	glVertex3d(0, 0, 2);
	glVertex3d(0, 1, 0);
	glVertex3d(0, 0, 0);

	glColor3f(1, 1, 0);
	glVertex3d(1, 0, 0);
	glVertex3d(0, 1, 0);
	glVertex3d(0, 0, 2);
	*/


	glEnd();
	displayB();
	glFlush();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
		exit(0);
		break;
	case 'a':
		glMatrixMode(GL_MODELVIEW);
		glRotatef(-10, 0, 0, 1);
		break;
	case 'd':
		glMatrixMode(GL_MODELVIEW);
		glRotatef(10, 0, 0, 1);
		break;
	case 'w':
		glMatrixMode(GL_MODELVIEW);
		glRotatef(-10, 0, 1, 0);
		break;
	case 's':
		glMatrixMode(GL_MODELVIEW);
		glRotatef(10, 0, 1, 0);
		break;
	case '-':
		glMatrixMode(GL_MODELVIEW);
		glScalef(0.9f, 0.9f, 0.9f);
		break;
	case '=':
		glMatrixMode(GL_MODELVIEW);
		glScalef(1.1f, 1.1f, 1.1f);
		break;
	case 'i':
		glMatrixMode(GL_MODELVIEW);
		glTranslatef(0, 0, 1);
		break;
	case 'k':
		glTranslatef(0, 0, -1);
		break;
	case 'j':
		glTranslatef(0, -1, 0);
		break;
	case 'l':
		glTranslatef(0, 1, 0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}
void init() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(100, 1, 0.5, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		1, 0, 6,
		0, 0, 0,
		0, 0, 1);
}
int main(int argc, char** argv) {
	testcase();

	glutInit(&argc, argv);
	glutInitDisplayMode(
		GLUT_SINGLE |
		GLUT_RGB);
	glutInitWindowSize(1080, 1080);
	glutInitWindowPosition(700, 0);
	glutCreateWindow("B spline");
	init();
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutMouseFunc(mouseCB);
	glutMotionFunc(mouseMotionCB);
	glutMainLoop();
	return 0;
}
