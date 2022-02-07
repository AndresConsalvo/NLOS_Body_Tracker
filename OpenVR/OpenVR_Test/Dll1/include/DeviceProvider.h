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
	TrackerDriver* NLOS_Tracker;
};