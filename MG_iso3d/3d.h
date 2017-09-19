#pragma once

#include <cmath>

// transform functions
inline void rotatepoint(double& ax1, double& ax2, double rot) {
	static double xt, yt, th;
	th = M_PI/180 * rot;
	xt = ax1*cos(th) - ax2*sin(th);
	yt = ax1*sin(th) + ax2*cos(th);
	ax1=xt, ax2=yt;
}
inline void transform(char axis, double delta, double* p) {
	switch(axis) {
	case 'r':  rotatepoint(p[0], p[1], delta);  break;
	case 'p':  rotatepoint(p[1], p[2], delta);  break;
	case 'Y':  rotatepoint(p[0], p[2], delta);  break;
	case 'x':  p[0] += delta;  break;
	case 'y':  p[1] += delta;  break;
	case 'z':  p[2] += delta;  break;
	case 's':  p[0]*=delta, p[1]*=delta, p[2]*=delta;  break;
	}
}
inline void transformln(char axis, double delta, double* p) {
	transform(axis, delta, p);
	transform(axis, delta, p+3);
}