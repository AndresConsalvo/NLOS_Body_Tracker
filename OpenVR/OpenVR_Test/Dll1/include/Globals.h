#pragma once

#include <openvr_driver.h>

#include <TrackerDefines.h>
#include <chrono>

using namespace vr;

//Network
extern bool SocketActivated;

extern DriverPose_t hmd_pose;

extern DriverPose_t waist_pose;
extern DriverPose_t lfoot_pose;
extern DriverPose_t rfoot_pose;
extern DriverPose_t lthigh_pose;
extern DriverPose_t rthigh_pose;

// Offsets
extern const double Head_to_Floor_len_m;
extern const double Head_to_Waist_len_m;
extern const double Waist_to_Foot_len_m;
