#pragma once

#include <Globals.h>
#include <UDP_receiver.h>
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

	UDP* udpThread = nullptr;

	TrackerDriver* chest_tracker = nullptr;
	TrackerDriver* waist_tracker = nullptr;
	TrackerDriver* lknee_tracker = nullptr;
	TrackerDriver* rknee_tracker = nullptr;
	TrackerDriver* lfoot_tracker = nullptr;
	TrackerDriver* rfoot_tracker = nullptr;
};