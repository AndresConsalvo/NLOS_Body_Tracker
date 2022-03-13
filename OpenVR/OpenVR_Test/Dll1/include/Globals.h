#pragma once

#include <openvr_driver.h>
#include <Matrix.h>
#include <TrackerDefines.h>
#include <chrono>

using namespace vr;

// Network
extern bool SocketActivated;

// Positioning of "left and right" is determined by the positioning of the limbs of a person facing away from you.
// Tracked Poses
extern DriverPose_t hmd_pose;

extern DriverPose_t waist_pose;
extern DriverPose_t lfoot_pose;
extern DriverPose_t rfoot_pose;
extern DriverPose_t lthigh_pose;
extern DriverPose_t rthigh_pose;

// Skeleton
extern DriverPose_t neck_pose;
extern DriverPose_t tailbone_pose;
extern DriverPose_t lhip_pose;
extern DriverPose_t rhip_pose;
// = q (4 floats) 3d position (3 floats) x, y, z

// Offsets
extern const double HMD_to_Head;
extern const double Head_to_Neck;

extern const double Neck_to_Waist;

extern const double Waist_to_Hip;
extern const double hip_width;

extern const double Head_to_Floor_len_m;
extern const double Head_to_Waist_len_m;
extern const double Hip_to_Foot_len_m;
