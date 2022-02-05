#include <TrackerDriver.h>

EVRInitError TrackerDriver::Activate(uint32_t unObjectId) {
	objID = unObjectId;

	// Props. Like JavaScript?
	ulPropertyContainer = VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);

	// Tracked device is opting out of left/right hand selection -- According to API documentation
	VRProperties()->SetInt32Property(ulPropertyContainer, Prop_ControllerRoleHint_Int32, ETrackedControllerRole::TrackedControllerRole_OptOut);
	VRProperties()->SetInt32Property(ulPropertyContainer, Prop_DeviceClass_Int32, TrackedDeviceClass_GenericTracker);

	// This describes the path to get to the inputprofile which is ABSOLUTELY NECESSARY.
	// Testing from earlier showed that even if it shouldn't be running the driver is still detected. Maybe when it runs it, reinstall the profile each time?
	VRProperties()->SetStringProperty(ulPropertyContainer, Prop_InputProfilePath_String, "{NLOS}/input/tracker_profile.json");

	VRProperties()->SetBoolProperty(ulPropertyContainer, Prop_HasDisplayComponent_Bool, false);
	VRProperties()->SetBoolProperty(ulPropertyContainer, Prop_HasCameraComponent_Bool, false);
	VRProperties()->SetBoolProperty(ulPropertyContainer, Prop_HasDriverDirectModeComponent_Bool, false);
	VRProperties()->SetBoolProperty(ulPropertyContainer, Prop_HasVirtualDisplayComponent_Bool, false);

	// Ignored as hand assignment
	VRProperties()->SetInt32Property(ulPropertyContainer, Prop_ControllerHandSelectionPriority_Int32, -1);

	return VRInitError_None;
}

void TrackerDriver::Deactivate() {
	objID = vr::k_unTrackedDeviceIndexInvalid;
}

void TrackerDriver::EnterStandby() {
	// Don't mess with this.
}

void* TrackerDriver::GetComponent(const char* pchComponentNameandVersion) {
	if (strcmp(IVRDisplayComponent_Version, pchComponentNameandVersion) == 0) {
		return (IVRDisplayComponent*) this;
	}

	return NULL;
}

void TrackerDriver::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) {
	if (unResponseBufferSize >= 1) {
		pchResponseBuffer[0] = 0;
	}
}

DriverPose_t TrackerDriver::GetPose() {
	DriverPose_t pose = { 0 };
	bool SocketActivated = 1;

	// Let's try tracking the left foot. Gotta start somewhere, right?
	// If I can track one thing I can track others.
	// First check if device is connected...

	// If everything works here my foot should bug out.
	// This is a good thing, as that means that it's connected with OpenVR.

	if (SocketActivated) {
		pose.poseIsValid = true;
		pose.result = TrackingResult_Running_OK;
		pose.deviceIsConnected = true;
	}
	else {
		pose.poseIsValid = false;
		pose.result = TrackingResult_Uninitialized;
		pose.deviceIsConnected = false;
	}

	HmdQuaternion_t quat;
	quat.x, quat.y, quat.z = 0;
	quat.w = 1;

	pose.qWorldFromDriverRotation = quat;
	pose.qDriverFromHeadRotation = quat;

	pose.vecPosition[0] = 0;
	pose.vecPosition[1] = 1;
	pose.vecPosition[2] = 0;

	pose.qRotation.x = 0;
	pose.qRotation.y = 0;
	pose.qRotation.z = 0;
	pose.qRotation.w = 1;

	return pose;
}

void TrackerDriver::RunFrame() {
	if (objID != k_unTrackedDeviceIndexInvalid) {
		VRServerDriverHost()->TrackedDevicePoseUpdated(objID, GetPose(), sizeof(DriverPose_t));
	}
}