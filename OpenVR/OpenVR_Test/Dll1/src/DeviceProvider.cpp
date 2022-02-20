#include <DeviceProvider.h>

bool waist_en = true;
bool lfoot_en = false;
bool rfoot_en = false;
bool lthigh_en = false;
bool rthigh_en = false;

EVRInitError DeviceProvider::Init(IVRDriverContext* pDriverContext) {
	EVRInitError initError = InitServerDriverContext(pDriverContext);

	if (initError != EVRInitError::VRInitError_None) {
		return initError;
	}

	

	if (waist_en) {
		VRDriverLog()->Log("Initializing waist tracker...");
		waist_tracker = new TrackerDriver();
		waist_tracker->setIndex(WAIST);
		VRServerDriverHost()->TrackedDeviceAdded("NLOS_Waist_Tracker", TrackedDeviceClass_GenericTracker, waist_tracker);
	}

	if (lfoot_en) {
		lfoot_tracker = new TrackerDriver();
		lfoot_tracker->setIndex(LFOOT);
		VRServerDriverHost()->TrackedDeviceAdded("NLOS Left Foot Tracker", TrackedDeviceClass_GenericTracker, lfoot_tracker);
	}

	if (rfoot_en) {
		rfoot_tracker = new TrackerDriver();
		rfoot_tracker->setIndex(RFOOT);
		VRServerDriverHost()->TrackedDeviceAdded("NLOS Right Foot Tracker", TrackedDeviceClass_GenericTracker, rfoot_tracker);
	}

	if (lthigh_en) {
		lthigh_tracker = new TrackerDriver();
		lthigh_tracker->setIndex(LTHIGH);
		VRServerDriverHost()->TrackedDeviceAdded("NLOS Right Foot Tracker", TrackedDeviceClass_GenericTracker, lthigh_tracker);
	}

	if (rthigh_en) {
		rthigh_tracker = new TrackerDriver();
		rthigh_tracker->setIndex(RTHIGH);
		VRServerDriverHost()->TrackedDeviceAdded("NLOS Right Foot Tracker", TrackedDeviceClass_GenericTracker, rthigh_tracker);
	}


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