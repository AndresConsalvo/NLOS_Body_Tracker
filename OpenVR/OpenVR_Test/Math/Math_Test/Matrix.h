#pragma once

#define _USE_MATH_DEFINES

#include <cmath>
#pragma comment(lib, "ws2_32.lib")

// 4x4 Matrix:
// 0 1 2 3
// 4 5 6 7
// 8 9 10 11
// 12 13 14 15

// This is an identity matrix.





struct ang_rate {
	double x, y, z;

	ang_rate(double ang_x, double ang_y, double ang_z);

	double get_magnitude();
};


struct omega {
	double* matrix = new double[16];

	double iMatrix[16] = { 1, 0, 0, 0,
							0, 1, 0, 0,
							0, 0, 1, 0,
							0, 0, 0, 1 };

	omega(ang_rate measured_angle);

	double* getMatrix();
};