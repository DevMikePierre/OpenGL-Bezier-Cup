#ifndef _VECTOR3_H_
#define _VECTOR3_H_

#include <string>
#include "Vector4.h"
#include <math.h>
using namespace std;

class Vector3 {

	public:
		float x, y, z;
		Vector3();
		Vector3(float, float, float);
		void set(float, float, float);
		void setX(float);
		void setY(float);
		void setZ(float);
		float get(int);
		float getX();
		float getY();
		float getZ();
		float operator[](const int i);
		void add(Vector3&);
		void add(Vector3 &a, Vector3 &b);
		Vector3 operator+(const Vector3 &other) const;
		void subtract(Vector3 &a);
		void subtract(Vector3 &a, Vector3 &b);
		Vector3 operator-(const Vector3 &other) const;
		void divide(float a);
		void negate();
		void negate(Vector3 &a);
		void scale(float s);
		void scale(float s, Vector3 &a);
		float dot(Vector3 &a);
		void cross(Vector3*);
		void cross(Vector3 &a, Vector3 &b);
		float angle(Vector3*);
		float magnitude();
		void normalize();
		float dist(Vector3 &a, Vector3 &b);
		void print();
		string toString();
};

#endif