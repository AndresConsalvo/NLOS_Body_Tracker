#include <DeviceProvider.h>

EVRInitError DeviceProvider::Init(IVRDriverContext* pDriverContext) {
	EVRInitError initError = InitServerDriverContext(pDriverContext);

	if (initError != EVRInitError::VRInitError_None) {
		return initError;
	}

	VRDriverLog()->Log("Initializing test tracker...");

	waist_tracker = new TrackerDriver();
	waist_tracker->setIndex(WAIST);
	VRServerDriverHost()->TrackedDeviceAdded("NLOS Waist Tracker", TrackedDeviceClass_GenericTracker, waist_tracker);

	return VRInitError_None;
}

void DeviceProvider::Cleanup() {
	delete waist_tracker;
	waist_tracker = NULL;

	delete lfoot_tracker;
	lfoot_tracker = NULL;

	delete rfoot_tracker;
	rfoot_tracker = NULL;

	delete lthigh_tracker;
	lthigh_tracker = NULL;

	delete rthigh_tracker;
	rthigh_tracker = NULL;
}

const char* const* DeviceProvider::GetInterfaceVersions() {
	return k_InterfaceVersions;
}

void DeviceProvider::RunFrame() {
	if (waist_tracker) {
		waist_tracker->RunFrame();
	}

	if (lfoot_tracker) {
		lfoot_tracker->RunFrame();
	}

	if (rfoot_tracker) {
		rfoot_tracker->RunFrame();
	}

	if (lthigh_tracker) {
		lthigh_tracker->RunFrame();
	}

	if (rthigh_tracker) {
		rthigh_tracker->RunFrame();
	}
}

bool DeviceProvider::ShouldBlockStandbyMode() {
	return false;
}

void DeviceProvider::EnterStandby() {}

void DeviceProvider::LeaveStandby() {}