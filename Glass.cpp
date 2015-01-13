#include "Glass.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <GL/glut.h>
#include <math.h>

using namespace std;
static float coord[7][3] = {
		{ 1.5, 3, 0 }, { 2, 1, 0 }, { 1, -.8, 0 }, { .2, -0.5, 0.0 },
		{ .1, -2, 0 }, { -.2, -4, 0 }, { 1.3, -4.5, 0.0 } };
float points[45][45][3];
static const float max_range = 1.0;
static const int num_points = 20;
static float inter[num_points][3];
static float inter2[num_points][3];
static float inter_ymax = FLT_MIN;
static float inter_ymin = FLT_MAX;
static float normal[num_points][3];
static float normal2[num_points][3];
float alpha = 1;
int depth = 1;
static Glass glass;
static Glass flag;
static bool left_clicked = false;
static int x_mouse;
static int y_mouse;
static const int degs = 5;
int Window::width = 512;   // set window width in pixels here
int Window::height = 512;   // set window height in pixels here
static float rotated_inter[360 / degs][num_points][3];
static float rotated_inter2[360 / degs][num_points][3];
static float rotated_normal[360 / degs][num_points][3];
static float rotated_normal2[360 / degs][num_points][3];
static float calc = -3.14*degs / 180;

void Window::idleCallback(void)
{
	displayCallback();
}

void Window::reshapeCallback(int w, int h)
{

	width = w;
	height = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-10.0, 10.0, -10.0, 10.0, 10, 1000.0); // set perspective projection viewing frustum
	glTranslatef(0, 0, -20);
}



Matrix4& Glass::getModelMatrix() {
	return glass.model;
}





Matrix4& Glass::getModelViewMatrix() {
	return glass.modelview;
}

void Glass::updateModelViewMatrix() {
	Matrix4 cam_inv = Matrix4(glass.getCameraMatrix());
	cam_inv.inverse();
	glass.getModelViewMatrix() = cam_inv.multiply(glass.getModelMatrix());
}

//----------------------------------------------------------------------------
// Callback method called when window readraw is necessary or
// when glutPostRedisplay() was called.
void Window::displayCallback(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear color and depth buffers

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glass.getModelViewMatrix().getGLMatrix());

	//control points
	glBegin(GL_POINTS);
	glColor3f(10, 10, 10);
	for (int i = 0; i < 7; i++) {
		glVertex3f(coord[i][0], coord[i][1], coord[i][2]);
	}
	glEnd();

	// connect control points
	for (int i = 0; i < 6; i++) {
		glBegin(GL_LINES);
		glColor3f(1, 1, 1);

		glVertex3f(coord[i][0], coord[i][1], coord[i][2]);
		glVertex3f(coord[i + 1][0], coord[i + 1][1], coord[i + 1][2]);
		glEnd();
	}

	glass.calculateBezier();
	glass.calculateBezierSurface();
	glass.drawBezier();
	glass.drawBezierSurface();
	glass.drawFlag();

	

	glFlush();
	glutSwapBuffers();
}


void Glass::calculateBezierSurface() {
	Matrix4 temp = Matrix4();
	Matrix4 temp2 = Matrix4();

	int k = 0;
	for (int j = 0; j<360; j += degs) {
		for (int i = 0; i<num_points; i++) {
			// calculate bezier surface 1

			// rotate interpolated points
			temp.identity();
			temp.set(3, 0, inter[i][0]);
			temp.set(3, 1, inter[i][1]);
			temp.set(3, 2, inter[i][2]);

			temp.rotateY(calc);

			rotated_inter[k][i][0] = temp.get(3, 0);
			rotated_inter[k][i][1] = temp.get(3, 1);
			rotated_inter[k][i][2] = temp.get(3, 2);

			// rotate normals
			temp2.identity();
			temp2.set(3, 0, normal[i][0]);
			temp2.set(3, 1, normal[i][1]);
			temp2.set(3, 2, normal[i][2]);

			temp2.rotateY(calc);

			rotated_normal[k][i][0] = temp2.get(3, 0);
			rotated_normal[k][i][1] = temp2.get(3, 1);
			rotated_normal[k][i][2] = temp2.get(3, 2);

			// update interpolated points and normals for next iteration
			inter[i][0] = temp.get(3, 0);
			inter[i][1] = temp.get(3, 1);
			inter[i][2] = temp.get(3, 2);

			normal[i][0] = temp2.get(3, 0);
			normal[i][1] = temp2.get(3, 1);
			normal[i][2] = temp2.get(3, 2);

			// calculate bezier surface 2

			// rotate interpolated points
			temp.identity();
			temp.set(3, 0, inter2[i][0]);
			temp.set(3, 1, inter2[i][1]);
			temp.set(3, 2, inter2[i][2]);

			temp.rotateY(calc);

			rotated_inter2[k][i][0] = temp.get(3, 0);
			rotated_inter2[k][i][1] = temp.get(3, 1);
			rotated_inter2[k][i][2] = temp.get(3, 2);

			// rotate normals
			temp2.identity();
			temp2.set(3, 0, normal2[i][0]);
			temp2.set(3, 1, normal2[i][1]);
			temp2.set(3, 2, normal2[i][2]);

			temp2.rotateY(calc);

			rotated_normal2[k][i][0] = temp2.get(3, 0);
			rotated_normal2[k][i][1] = temp2.get(3, 1);
			rotated_normal2[k][i][2] = temp2.get(3, 2);

			// update interpolated points and normals for next iteration
			inter2[i][0] = temp.get(3, 0);
			inter2[i][1] = temp.get(3, 1);
			inter2[i][2] = temp.get(3, 2);

			normal2[i][0] = temp2.get(3, 0);
			normal2[i][1] = temp2.get(3, 1);
			normal2[i][2] = temp2.get(3, 2);
		}
		k++;
	}

}
void Glass::calculateBezier()
{
	Matrix4 G = Matrix4(
		coord[0][0], coord[0][1], coord[0][2], 0,
		coord[1][0], coord[1][1], coord[1][2], 0,
		coord[2][0], coord[2][1], coord[2][2], 0,
		coord[3][0], coord[3][1], coord[3][2], 0);
	Matrix4 G2 = Matrix4(
		coord[3][0], coord[3][1], coord[3][2], 0,
		coord[4][0], coord[4][1], coord[4][2], 0,
		coord[5][0], coord[5][1], coord[5][2], 0,
		coord[6][0], coord[6][1], coord[6][2], 0);
	Matrix4 B = Matrix4(-1, 3, -3, 1, 3, -6, 3, 0, -3, 3, 0, 0, 1, 0, 0, 0);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(2);

	// calculate bezier curve 1
	Matrix4 C = G.multiply(B);
	Vector4 tmp;
	Vector4 res;
	Vector3 tmp2;

	float interval = max_range / num_points;
	int j = 0;
	for (float i = 0; i < max_range; i += interval) {
		// calculate interpolated point at t=i
		tmp = Vector4(i*i*i, i*i, i, 1);
		res = C.multiply(tmp);
		inter[j][0] = res[0];
		inter[j][1] = res[1];
		inter[j][2] = res[2];

		// calculate normal at interpolated point
		tmp = Vector4(3 * i*i, 2 * i, 1, 0);
		res = C.multiply(tmp);
		tmp2 = Vector3(-1 * res[1], res[0], 0);
		tmp2.normalize();
		normal[j][0] = tmp2.getX();
		normal[j][1] = tmp2.getY();
		normal[j][2] = tmp2.getZ();

		j++;
	}

	// calculate bezier curve 2
	Matrix4 C2 = G2.multiply(B);
	tmp2 = Vector3();
	tmp = Vector4();
	res = Vector4();

	interval = max_range / num_points;
	j = 0;
	for (float i = 0; i < max_range; i += interval) {
		// calculate interpolated point at t=i
		tmp = Vector4(i*i*i, i*i, i, 1);
		res = C2.multiply(tmp);
		inter2[j][0] = res[0];
		inter2[j][1] = res[1];
		inter2[j][2] = res[2];

		// calculate normal at interpolated point
		tmp = Vector4(3 * i*i, 2 * i, 1, 0);
		res = C2.multiply(tmp);
		tmp2 = Vector3(-1 * res[1], res[0], 0);
		tmp2.normalize();
		normal2[j][0] = tmp2.getX();
		normal2[j][1] = tmp2.getY();
		normal2[j][2] = tmp2.getZ();

		j++;
	}

}

void Glass::drawBezierSurface() {
	//glColor3f(1, 0, 0);
	glColor4f(1, 0, 0, alpha);

	float interval = (max_range) / num_points;
	int j = 0;

	for (int i = 0; i<360 / degs; i++) {

		int i_next = i + 1;
		if (i + 1 >= 360 / degs) i_next = 0;
		j = 0;
		GLfloat x0 = float(i) / float(360 / degs);
		GLfloat x1 = float(i + 1) / float(360 / degs);

		for (float k = 0; k<max_range; k += interval) {
			if (j<num_points - 1) {
				
				glBegin(GL_QUADS);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glDepthMask(depth);
				// draw bezier surface 1
				glNormal3f(rotated_normal[i][j][0], rotated_normal[i][j][1], rotated_normal[i][j][2]);
				glVertex3f(rotated_inter[i][j][0], rotated_inter[i][j][1], rotated_inter[i][j][2]);
				glNormal3f(rotated_normal[i][j + 1][0], rotated_normal[i][j + 1][1], rotated_normal[i][j + 1][2]);
				glVertex3f(rotated_inter[i][j + 1][0], rotated_inter[i][j + 1][1], rotated_inter[i][j + 1][2]);
				glNormal3f(rotated_normal[i_next][j + 1][0], rotated_normal[i_next][j + 1][1], rotated_normal[i_next][j + 1][2]);
				glVertex3f(rotated_inter[i_next][j + 1][0], rotated_inter[i_next][j + 1][1], rotated_inter[i_next][j + 1][2]);
				glNormal3f(rotated_normal[i_next][j][0], rotated_normal[i_next][j][1], rotated_normal[i_next][j][2]);
				glVertex3f(rotated_inter[i_next][j][0], rotated_inter[i_next][j][1], rotated_inter[i_next][j][2]);
				glEnd();
				
				glDepthMask(depth);
			}

			j++;
		}

		glNormal3f(rotated_normal[i][num_points - 1][0], rotated_normal[i][num_points - 1][1], rotated_normal[i][num_points - 1][2]);
		glVertex3f(rotated_inter[i][num_points - 1][0], rotated_inter[i][num_points - 1][1], rotated_inter[i][num_points - 1][2]);

		glNormal3f(rotated_normal2[i][0][0], rotated_normal2[i][0][1], rotated_normal2[i][0][2]);
		glVertex3f(rotated_inter2[i][0][0], rotated_inter2[i][0][1], rotated_inter2[i][0][2]);


		glNormal3f(rotated_normal2[i_next][0][0], rotated_normal2[i_next][0][1], rotated_normal2[i_next][0][2]);
		glVertex3f(rotated_inter2[i_next][0][0], rotated_inter2[i_next][0][1], rotated_inter2[i_next][0][2]);

		glNormal3f(rotated_normal[i_next][num_points - 1][0], rotated_normal[i_next][num_points - 1][1], rotated_normal[i_next][num_points - 1][2]);
		glVertex3f(rotated_inter[i_next][num_points - 1][0], rotated_inter[i_next][num_points - 1][1], rotated_inter[i_next][num_points - 1][2]);
		glEnd();
		
		j = 0;
		for (float k = interval; k<max_range; k += interval) {

			if (j<num_points) {
				glBegin(GL_QUADS);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				// draw bezier surface 
				glNormal3f(rotated_normal2[i][j][0], rotated_normal2[i][j][1], rotated_normal2[i][j][2]);
				glVertex3f(rotated_inter2[i][j][0], rotated_inter2[i][j][1], rotated_inter2[i][j][2]);

				glNormal3f(rotated_normal2[i][j + 1][0], rotated_normal2[i][j + 1][1], rotated_normal2[i][j + 1][2]);
				glVertex3f(rotated_inter2[i][j + 1][0], rotated_inter2[i][j + 1][1], rotated_inter2[i][j + 1][2]);


				glNormal3f(rotated_normal2[i_next][j + 1][0], rotated_normal2[i_next][j + 1][1], rotated_normal2[i_next][j + 1][2]);
				glVertex3f(rotated_inter2[i_next][j + 1][0], rotated_inter2[i_next][j + 1][1], rotated_inter2[i_next][j + 1][2]);

				glNormal3f(rotated_normal2[i_next][j][0], rotated_normal2[i_next][j][1], rotated_normal2[i_next][j][2]);
				glVertex3f(rotated_inter2[i_next][j][0], rotated_inter2[i_next][j][1], rotated_inter2[i_next][j][2]);
				glEnd();
			}

			j++;
		}

	}

}

void Glass::drawBezier() {
	glEnable(GL_POINT_SMOOTH);
	glPointSize(5);

	// draw bezier curve 1
	int j = 0;
	for (float i = 0; i<max_range; i += max_range / num_points) {
		glBegin(GL_POINTS);
		glColor3f(0, 0, 1);

		glVertex3f(inter[j][0], inter[j][1], inter[j][2]);
		glEnd();
		j++;
	}

	for (int i = 0; i < num_points - 1; i++) {
		glBegin(GL_LINES);
		glColor3f(1, 1, 1);
		glVertex3f(inter[i][0], inter[i][1], inter[i][2]);
		glVertex3f(inter[i + 1][0], inter[i + 1][1], inter[i + 1][2]);
		glEnd();
	}

	// draw bezier curve 2
	j = 0;
	for (float i = 0; i<max_range; i += max_range / num_points) {
		glBegin(GL_POINTS);
		glColor3f(0, 0, 1);
		glVertex3f(inter2[j][0], inter2[j][1], inter2[j][2]);
		glEnd();
		j++;
	}

	for (int i = 0; i < num_points - 1; i++) {
		glBegin(GL_LINES);
		glColor3f(1, 1, 1);
		glVertex3f(inter2[i][0], inter2[i][1], inter2[i][2]);
		glVertex3f(inter2[i + 1][0], inter2[i + 1][1], inter2[i + 1][2]);
		glEnd();
	}

}

void Glass::drawFlag(){

	glScalef(.2, .2, .2);
	glColor3f(0, 1, 1);
	float float_x, float_y, float_xb, float_yb;
	for (int x = 0; x<45; x++)
	{
		for (int y = 0; y<45; y++)
		{
			/*points[x][y][0] = float((x / 25.0f)+1 );
			points[x][y][1] = float((y / 40.0f) +3.2 );
			points[x][y][2] = float(sin((((x /11.0f)*40.0f) / 360.0)*3.141592654*2.0f));*/
			points[x][y][0] = float((x / 4.0f) +3);
			points[x][y][1] = float((y / 5.0f) +15.2);
			points[x][y][2] = float(sin((((x / 4.0f)*40.0f) / 360.0f)*3.141592654*2.0f));
		}
	}
	glBegin(GL_QUADS);

	for (int x = 0; x < 44; x++)
	{
		for (int y = 0; y < 44; y++)
		{
			float_x = float(x) / 40.0f;
			float_y = float(y) / 44.0f;
			float_xb = float(x + 1) / 44.0f;
			float_yb = float(y + 1) / 44.0f;

			glTexCoord2f(float_x, float_y);
			glVertex3f(points[x][y][0], points[x][y][1], points[x][y][2]);

			glTexCoord2f(float_x, float_yb);
			glVertex3f(points[x][y + 1][0], points[x][y + 1][1], points[x][y + 1][2]);

			glTexCoord2f(float_xb, float_yb);
			glVertex3f(points[x + 1][y + 1][0], points[x + 1][y + 1][1], points[x + 1][y + 1][2]);

			glTexCoord2f(float_xb, float_y);
			glVertex3f(points[x + 1][y][0], points[x + 1][y][1], points[x + 1][y][2]);
		}
	
	}
	glEnd();
	glColor3f(1, 0, 1);
	glTranslated(3, 24, 0.0);
	glRotatef(90, 50.0f, 5.0f, 0.0f);
	GLUquadricObj *obj = gluNewQuadric();
	gluCylinder(obj, .75, .75, 20, 30, 30);
}



Glass::Glass() {
	glass.getModelMatrix().identity();
	glass.getModelMatrix() = glass.getModelMatrix().multiply(glass.setScaleMatrix(8.0));
	glass.getCameraMatrix().identity();
	glass.updateModelViewMatrix();
}

Matrix4& Glass::getCameraMatrix() {
	return glass.camera;
}
int main(int argc, char *argv[])
{
	float specular[] = { 1.0, 1.0, 1.0, 1.0 };
	float shininess[] = { 100.0 };

	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glutInit(&argc, argv);      	      	      
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
	glutInitWindowSize(Window::width, Window::height);    
	glutCreateWindow("OpenGL Cube for CS596");    	    
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);            	   
	glClear(GL_DEPTH_BUFFER_BIT);       	    
	glClearColor(0.0, 0.0, 0.0, 0.0);   	    
	glMatrixMode(GL_PROJECTION);
	GLfloat position[] = { 0.0, 1.0, 1.0, 0.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glutDisplayFunc(Window::displayCallback);
	glutReshapeFunc(Window::reshapeCallback);
	glutIdleFunc(Window::idleCallback);

	glEnable(GL_NORMALIZE);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	glutKeyboardFunc(Window::processNormalKeys);
	glutMouseFunc(Window::processMouseClick);
	glutMotionFunc(Window::processMouseMove);
	glass.getModelViewMatrix().translate(0, -10, -50);

	glutMainLoop();
	return 0;
}



Matrix4 Glass::setScaleMatrix(float factor) {
	Matrix4 scale = Matrix4();
	for (int i = 0; i < 3; i++)
	{
		scale.set(i, i, factor);
	}
	return scale;
}


void Window::processNormalKeys(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 't':
		if (alpha == 1){
			alpha = .4;
			depth = 0;
		}
		else{
			alpha = 1;
			depth = 1;
		}
		break;
	case 'x':
		// move cube left by a small amount
		glass.getModelViewMatrix().translate(-1, 0, 0);
		break;
	case 'X':
		// move cube right by a small amount
		glass.getModelViewMatrix().translate(1, 0, 0);
		break;
	case 'y':
		// move cube down by a small amount
		glass.getModelViewMatrix().translate(0, -1, 0);
		break;
	case 'Y':
		// move cube up by a small amount
		glass.getModelViewMatrix().translate(0, 1, 0);
		break;
	case 'z':
		// move cube into of the screen by a small amount
		glass.getModelViewMatrix().translate(0, 0, -1);
		break;
	case 'Z':
		// move cube out of the screen by a small amount
		glass.getModelViewMatrix().translate(0, 0, 1);
		break;
	}
}



void Window::processMouseClick(int button, int state, int x, int y) {

	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
			left_clicked = true;
			x_mouse = x;
			y_mouse = y;

		}
		else
		{
			left_clicked = false;

		}
	}
}

void Window::processMouseMove(int x, int y) {
	if (x != x_mouse || y != y_mouse) {
		glass.getModelViewMatrix().trackballRotation(Window::width, Window::height, x_mouse, y_mouse, x, y);
		x_mouse = x;
		y_mouse = y;

	}
}

