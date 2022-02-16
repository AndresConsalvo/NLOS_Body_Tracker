#pragma once


#define _USE_MATH_DEFINES

#include <Matrix.h>
#include <chrono>
#include <cmath>
#include <iostream>
#include <openvr_driver.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <thread>
#pragma comment(lib, "ws2_32.lib")




#define deg_to_rad(x) (x * (M_PI)/180.0)



using namespace vr;

class TrackerDriver : public ITrackedDeviceServerDriver {
public:
	// ------------------------------------
	// Management Methods
	// ------------------------------------
	/* This is called before an HMD is returned to the application. It will always be
	* called before any display or tracking methods. Memory and processor use by the
	* ITrackedDeviceServerDriver object should be kept to a minimum until it is activated.
	* The pose listener is guaranteed to be valid until Deactivate is called, but
	* should not be used after that point. */
	virtual EVRInitError Activate(uint32_t unObjectId);

	/** This is called when The VR system is switching from this Hmd being the active display
	* to another Hmd being the active display. The driver should clean whatever memory
	* and thread use it can when it is deactivated */
	void Deactivate();

	/** Handles a request from the system to put this device into standby mode. What that means is defined per-device. */
	void EnterStandby();

	/** Requests a component interface of the driver for device-specific functionality. The driver should return NULL
	* if the requested interface or version is not supported. */
	void* GetComponent(const char* pchComponentNameAndVersion);

	/** A VR Client has made this debug request of the driver. The set of valid requests is entirely
	* up to the driver and the client to figure out, as is the format of the response. Responses that
	* exceed the length of the supplied buffer should be truncated and null terminated */
	void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize);

	// ------------------------------------
	// Tracking Methods
	// ------------------------------------
	DriverPose_t GetPose();

	void RunFrame();

	// Self-made methods
	void UDP_init();

	

private:
	int32_t TrackerIndex = 0;

	std::string model = "Waist_Tracker";
	std::string version = "0.0.1"; // How do version numbers work?
	TrackedDeviceIndex_t objID = k_unTrackedDeviceIndexInvalid;
	PropertyContainerHandle_t ulPropertyContainer = k_ulInvalidPropertyContainer;

};