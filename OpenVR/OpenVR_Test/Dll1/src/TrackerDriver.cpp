#include <TrackerDriver.h>

EVRInitError TrackerDriver::Activate(uint32_t unObjectId) {
	driverID = unObjectId;
}

void TrackerDriver::Deactivate() {
	driverID = vr::k_unTrackedDeviceIndexInvalid;
}

void TrackerDriver::EnterStandby() {
	// Don't mess with this.
}

void* TrackerDriver::GetComponent(const char* pchComponentNameandVersion) {
	if (strcmp(vr::IVRDriverInput_Version, pchComponentNameandVersion) == 0) {
		return this;
	}

	return NULL;
}

void TrackerDriver::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) {
	if (unResponseBufferSize >= 1) {
		pchResponseBuffer[0] = 0;
	}
}

DriverPose_t TrackerDriver::GetPose() {
	// Let's try tracking the left foot. Gotta start somewhere, right?
}

void TrackerDriver::RunFrame() {
	if (driverID != k_unTrackedDeviceIndexInvalid) {
		VRServerDriverHost()->TrackedDevicePoseUpdated(driverID, GetPose(), sizeof(DriverPose_t));
	}
}