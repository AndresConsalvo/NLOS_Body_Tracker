#pragma once


#include <openvr_driver.h>
#include <Windows.h>


class ControllerDriver : public	vr::ITrackedDeviceServerDriver {
	
public:
	vr::EVRInitError Activate(uint32_t uObjectID);

	void Deactivate();

	void EnterStandby();

};
