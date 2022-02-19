#pragma once

#include <openvr_driver.h>
#include <TrackerDriver.h>



using namespace vr;

class DeviceProvider : public vr::IServerTrackedDeviceProvider {
public:
	vr::EVRInitError Init(vr::IVRDriverContext* pDriverContext);

	void Cleanup();

	const char* const* GetInterfaceVersions();

	// Updates drivers
	void RunFrame();

	bool ShouldBlockStandbyMode();

	void EnterStandby();

	void LeaveStandby();

private:
	bool waist_en = false;
	bool lfoot_en = false;
	bool rfoot_en = false;
	bool lthigh_en = false;
	bool rthigh_en = false;

	TrackerDriver* waist_tracker = nullptr;
	TrackerDriver* lfoot_tracker = nullptr;
	TrackerDriver* rfoot_tracker = nullptr;
	TrackerDriver* lthigh_tracker = nullptr;
	TrackerDriver* rthigh_tracker = nullptr;
};