#include <DeviceProvider.h>

bool waist_en = true;
bool lfoot_en = true;
bool rfoot_en = true;
bool lthigh_en = false;
bool rthigh_en = false;

bool SocketActivated = false;

DriverPose_t waist_pose = { 0 };
DriverPose_t lfoot_pose = { 0 };
DriverPose_t rfoot_pose = { 0 };

EVRInitError DeviceProvider::Init(IVRDriverContext* pDriverContext) {
	EVRInitError initError = InitServerDriverContext(pDriverContext);

	if (initError != EVRInitError::VRInitError_None) {
		return initError;
	}

	udpThread = new UDP();
	udpThread->init();

	

	if (waist_en) {
		VRDriverLog()->Log("Initializing waist tracker...");
		waist_pose.qRotation.w = 1.0;
		waist_pose.qRotation.x = 0.0;
		waist_pose.qRotation.y = 0.0;
		waist_pose.qRotation.z = 0.0;

		waist_tracker = new TrackerDriver();
		waist_tracker->setIndex(WAIST);

		waist_tracker->SetModel("Waist_Tracker");
		waist_tracker->SetVersion("0.0.1");

		VRServerDriverHost()->TrackedDeviceAdded("NLOS_Waist_Tracker", TrackedDeviceClass_GenericTracker, waist_tracker);
	}

	if (lfoot_en) {
		VRDriverLog()->Log("Initializing left foot tracker...");
		lfoot_pose.qRotation.w = 1.0;
		lfoot_pose.qRotation.x = 0.0;
		lfoot_pose.qRotation.y = 0.0;
		lfoot_pose.qRotation.z = 0.0;

		lfoot_tracker = new TrackerDriver();
		lfoot_tracker->setIndex(LFOOT);

		waist_tracker->SetModel("LFoot_Tracker");
		waist_tracker->SetVersion("0.0.1");

		VRServerDriverHost()->TrackedDeviceAdded("NLOS_LFoot_Tracker", TrackedDeviceClass_GenericTracker, lfoot_tracker);
	}

	if (rfoot_en) {
		VRDriverLog()->Log("Initializing right foot tracker...");
		rfoot_pose.qRotation.w = 1.0;
		rfoot_pose.qRotation.x = 0.0;
		rfoot_pose.qRotation.y = 0.0;
		rfoot_pose.qRotation.z = 0.0;

		rfoot_tracker = new TrackerDriver();
		rfoot_tracker->setIndex(RFOOT);

		rfoot_tracker->SetModel("RFoot_Tracker");
		rfoot_tracker->SetVersion("0.0.1");

		VRServerDriverHost()->TrackedDeviceAdded("NLOS_RFoot_Tracker", TrackedDeviceClass_GenericTracker, rfoot_tracker);
	}

	if (lthigh_en) {
		lthigh_tracker = new TrackerDriver();
		lthigh_tracker->setIndex(LTHIGH);
		VRServerDriverHost()->TrackedDeviceAdded("NLOS_LThigh_Tracker", TrackedDeviceClass_GenericTracker, lthigh_tracker);
	}

	if (rthigh_en) {
		rthigh_tracker = new TrackerDriver();
		rthigh_tracker->setIndex(RTHIGH);
		VRServerDriverHost()->TrackedDeviceAdded("NLOS_RThigh_Tracker", TrackedDeviceClass_GenericTracker, rthigh_tracker);
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