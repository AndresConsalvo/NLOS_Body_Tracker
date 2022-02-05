#include <DeviceProvider.h>

EVRInitError DeviceProvider::Init(IVRDriverContext* pDriverContext) {
	EVRInitError initError = InitServerDriverContext(pDriverContext);

	if (initError != EVRInitError::VRInitError_None) {
		return initError;
	}

	VRDriverLog()->Log("Initializing test tracker...");

	NLOS_Tracker = new TrackerDriver();
	VRServerDriverHost()->TrackedDeviceAdded("NLOS_Tracker", TrackedDeviceClass_GenericTracker, NLOS_Tracker);

	return VRInitError_None;
}

void DeviceProvider::Cleanup() {
	delete NLOS_Tracker;
	NLOS_Tracker = NULL;
}

const char* const* DeviceProvider::GetInterfaceVersions() {
	return k_InterfaceVersions;
}

void DeviceProvider::RunFrame() {
	NLOS_Tracker->RunFrame();
}

bool DeviceProvider::ShouldBlockStandbyMode() {
	return false;
}

void DeviceProvider::EnterStandby() {}

void DeviceProvider::LeaveStandby() {}