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

	HmdQuaternion_t quat;
	quat.w = 1.0;
	quat.x = 0.0;
	quat.y = 0.0;
	quat.z = 0.0;

	if (waist_en) {
		VRDriverLog()->Log("Initializing waist tracker...");

		waist_pose.qRotation = quat;
		waist_pose.qWorldFromDriverRotation = quat;
		waist_pose.qDriverFromHeadRotation = quat;

		waist_tracker = new TrackerDriver();
		waist_tracker->setIndex(WAIST);

		waist_tracker->SetModel("Waist_Tracker");
		waist_tracker->SetVersion("0.0.1");

		VRServerDriverHost()->TrackedDeviceAdded("NLOS_Waist_Tracker", TrackedDeviceClass_GenericTracker, waist_tracker);
	}

	if (lknee_en) {
		VRDriverLog()->Log("Initializing lknee tracker...");

		lknee_pose.qRotation = quat;
		lknee_pose.qWorldFromDriverRotation = quat;
		lknee_pose.qDriverFromHeadRotation = quat;

		lknee_tracker = new TrackerDriver();
		lknee_tracker->setIndex(LKNEE);

		waist_tracker->SetModel("LKnee_Tracker");
		waist_tracker->SetVersion("0.0.1");

		VRServerDriverHost()->TrackedDeviceAdded("NLOS_LThigh_Tracker", TrackedDeviceClass_GenericTracker, lknee_tracker);
	}

	if (rknee_en) {
		VRDriverLog()->Log("Initializing rknee tracker...");

		rknee_pose.qRotation = quat;
		rknee_pose.qWorldFromDriverRotation = quat;
		rknee_pose.qDriverFromHeadRotation = quat;

		rknee_tracker = new TrackerDriver();
		rknee_tracker->setIndex(RKNEE);

		waist_tracker->SetModel("RKnee_Tracker");
		waist_tracker->SetVersion("0.0.1");

		VRServerDriverHost()->TrackedDeviceAdded("NLOS_RThigh_Tracker", TrackedDeviceClass_GenericTracker, rknee_tracker);
	}

	if (lfoot_en) {
		VRDriverLog()->Log("Initializing left foot tracker...");

		lfoot_pose.qRotation = quat;
		lfoot_pose.qWorldFromDriverRotation = quat;
		lfoot_pose.qDriverFromHeadRotation = quat;

		lfoot_tracker = new TrackerDriver();
		lfoot_tracker->setIndex(LFOOT);

		waist_tracker->SetModel("LFoot_Tracker");
		waist_tracker->SetVersion("0.0.1");

		VRServerDriverHost()->TrackedDeviceAdded("NLOS_LFoot_Tracker", TrackedDeviceClass_GenericTracker, lfoot_tracker);
	}

	if (rfoot_en) {
		VRDriverLog()->Log("Initializing right foot tracker...");

		rfoot_pose.qRotation = quat;
		rfoot_pose.qWorldFromDriverRotation = quat;
		rfoot_pose.qDriverFromHeadRotation = quat;

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