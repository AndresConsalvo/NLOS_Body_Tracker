#pragma once


#include <openvr_driver.h>
#include <Windows.h>


class ControllerDriver : public	vr::ITrackedDeviceServerDriver {
	
public:
	vr::EVRInitError Activate(uint32_t uObjectID);

	void Deactivate();

	void EnterStandby();

	void* GetComponent(const char* pchComponentNameandVersion);

	void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize);

	vr::DriverPose_t GetPose();

	void RunFrame();

private:
	
	uint32_t driverID;
	vr::VRInputComponentHandle_t joystickYHandle;
	vr::VRInputComponentHandle_t trackpadYHandle;
	vr::VRInputComponentHandle_t joystickXHandle;
	vr::VRInputComponentHandle_t trackpadXHandle;
};
