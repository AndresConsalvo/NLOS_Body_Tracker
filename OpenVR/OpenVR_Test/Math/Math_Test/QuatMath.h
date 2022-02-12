#pragma once

#define _USE_MATH_DEFINES

#include <cmath>
#pragma comment(lib, "ws2_32.lib")

#define deg_to_rad(x) (x * (M_PI)/180.0)

class Quat {
	double x, y, z = 0;
	double w = 1;
};