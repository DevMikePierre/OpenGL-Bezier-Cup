#ifndef _GLASS_H_
#define _GLASS_H_

#include "Vector3.h"
#include "Matrix4.h"

class Glass
{
public:
	Matrix4 model;
	Matrix4 camera;
	Matrix4 modelview;
	Glass();
	Glass(Matrix4&);
	Matrix4& getCameraMatrix();
	Matrix4& getModelMatrix();
	Matrix4& getModelViewMatrix();
	Matrix4& getViewportMatrix();
	Matrix4& getProjectionMatrix();
	void setViewportMatrix();
	void setProjectionMatrix();
	void updateModelViewMatrix();
	Matrix4 setScaleMatrix(float);
	void calculateBezierSurface();
	void drawBezierSurface();
	void calculateBezier();
	void drawBezier();
	void drawFlag();
	void drawFlag2();


};

class Window	  // output window related routines
{
public:
	static int width, height; 	            // window size
	static void idleCallback(void);
	static void reshapeCallback(int, int);
	static void displayCallback(void);
	static void processNormalKeys(unsigned char, int, int);
	static void processMouseClick(int, int, int, int);
	static void processMouseMove(int, int);

};

#endif