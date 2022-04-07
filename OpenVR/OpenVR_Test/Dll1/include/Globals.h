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

extern DriverPose_t chest_pose;
extern DriverPose_t waist_pose;
extern DriverPose_t lknee_pose;
extern DriverPose_t rknee_pose;
extern DriverPose_t lfoot_pose;
extern DriverPose_t rfoot_pose;

// = q (4 floats) 3d position (3 floats) x, y, z

// Offsets (Legacy, not needed anymore)
extern const double HMD_to_Head;
extern double Head_to_Neck;

extern double Neck_to_Waist;

extern const double Waist_to_Hip;
extern const double hip_width;

extern const double Head_to_Floor_len_m;
extern const double Head_to_Waist_len_m;
extern double Hip_to_Foot_len_m;

extern double ankle_to_ground;
