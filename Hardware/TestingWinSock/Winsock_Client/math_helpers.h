#pragma once

#define _USE_MATH_DEFINES

#include <cmath>
#pragma comment(lib, "ws2_32.lib")
#include "Matrix.h"

#define deg_to_rad(x) (x * (M_PI)/180.0)
#define rad_to_deg(x) (x * 180.0 / M_PI);

// 4x4 Matrix:
// 0 1 2 3
// 4 5 6 7
// 8 9 10 11
// 12 13 14 15

// This is an identity matrix.


//void getNewPose(int limb, ang_rate angle_vector, double elapsed_time_ms);
