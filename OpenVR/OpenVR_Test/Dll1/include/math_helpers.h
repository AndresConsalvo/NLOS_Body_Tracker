#pragma once

#define _USE_MATH_DEFINES

#include <openvr_driver.h>
#include <cmath>
#include <Globals.h>
#pragma comment(lib, "ws2_32.lib")

#define deg_to_rad(x) (x * (M_PI)/180.0)


using namespace vr;

void getNewPose(int limb, Vector3_d angle_vector, double elapsed_time_ms);

void updateSkeleton();

bool checkIfZero(double& val, double threshold);

Quaternion getQuaternionFromHMD(HmdMatrix34_t& matrix);