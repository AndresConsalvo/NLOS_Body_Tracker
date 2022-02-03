#pragma once

#include <openvr_driver.h>
#include <Windows.h>
#include <ControllerDriver.h>



class DeviceProvider : public	vr::IServerTrackedDeviceProvider {
public:
	vr::EVRInitError init(vr::IVRDriverContext* pDriverContext);

	void Cleanup();

	const char* const* GetInterfaceversions();

	// Updates drivers
	void RunFrame();

	bool ShouldBlockStandbyMode();

	void EnterStandby();

	void LeaveStandby();
};