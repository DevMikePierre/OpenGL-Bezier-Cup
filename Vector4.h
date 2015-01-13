#ifndef _VECTOR4_H_
#define _VECTOR4_H_

#include <string>


using namespace std;
class Vector4 {
	public:
		float x, y, z, w;
		Vector4();
		Vector4(float, float, float);
		Vector4(float, float, float, float);
		void set(float, float, float, float);
		void setX(float);
		void setY(float);
		void setZ(float);
		void setW(float);
		float get(int);
		float getX();
		float getY();
		float getZ();
		float getW();
		float operator[] (const int);
		void add(Vector4&);
		void add(Vector4&, Vector4&);
		Vector4 operator+(const Vector4&) const;
		void subtract(Vector4&);
		void subtract(Vector4&, Vector4&);
		Vector4 operator-(const Vector4&) const;
		void dehomogenize();
		void scale(float s);
		void scale(float s, Vector4 &a);
		float magnitude();
		void normalize();
		void print();
		string toString();
		
};

#endif