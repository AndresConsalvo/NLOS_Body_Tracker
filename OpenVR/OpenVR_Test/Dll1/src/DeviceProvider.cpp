#include <DeviceProvider.h>

bool waist_en = true;
bool lknee_en = false;
bool rknee_en = false;
bool lfoot_en = true;
bool rfoot_en = true;

bool SocketActivated = false;

DriverPose_t hmd_pose = { 0 };
DriverPose_t waist_pose = { 0 };
DriverPose_t lknee_pose = { 0 };
DriverPose_t rknee_pose = { 0 };
DriverPose_t lfoot_pose = { 0 };
DriverPose_t rfoot_pose = { 0 };

EVRInitError DeviceProvider::Init(IVRDriverContext* pDriverContext) {
	EVRInitError initError = InitServerDriverContext(pDriverContext);

	if (initError != EVRInitError::VRInitError_None) {
		return initError;
	}

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

	if (lknee_en) {
		lknee_tracker = new TrackerDriver();
		lknee_tracker->setIndex(LKNEE);
		VRServerDriverHost()->TrackedDeviceAdded("NLOS_LThigh_Tracker", TrackedDeviceClass_GenericTracker, lknee_tracker);
	}

	if (rknee_en) {
		rknee_tracker = new TrackerDriver();
		rknee_tracker->setIndex(RKNEE);
		VRServerDriverHost()->TrackedDeviceAdded("NLOS_RThigh_Tracker", TrackedDeviceClass_GenericTracker, rknee_tracker);
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



	udpThread = new UDP();
	udpThread->init();

	return VRInitError_None;
}

void DeviceProvider::Cleanup() {
	delete waist_tracker;
	waist_tracker = NULL;

	delete lfoot_tracker;
	lfoot_tracker = NULL;

	delete rfoot_tracker;
	rfoot_tracker = NULL;

	delete lknee_tracker;
	lknee_tracker = NULL;

	delete rknee_tracker;
	rknee_tracker = NULL;
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

	if (lknee_tracker) {
		lknee_tracker->RunFrame();
	}

	if (rknee_tracker) {
		rknee_tracker->RunFrame();
	}
}

bool DeviceProvider::ShouldBlockStandbyMode() {
	return false;
}

void DeviceProvider::EnterStandby() {}

void DeviceProvider::LeaveStandby() {}