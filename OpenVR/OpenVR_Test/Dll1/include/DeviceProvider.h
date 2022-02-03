#pragma once

#include <openvr_driver.h>
#include <Windows.h>
#include <ControllerDriver.h>



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
	ControllerDriver* controllerDriver;
};