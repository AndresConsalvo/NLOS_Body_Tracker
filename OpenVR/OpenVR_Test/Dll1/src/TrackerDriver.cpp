#include <TrackerDriver.h>

EVRInitError TrackerDriver::Activate(uint32_t unObjectId) {
	char log_str[100];
	// OpenVR automatically assigns this a unique object ID.
	objID = unObjectId;
	snprintf(log_str, 100, "Tracker ID: %d", objID);
	VRDriverLog()->Log(log_str);
	

	// Props. Like JavaScript?
	ulPropertyContainer = VRProperties()->TrackedDeviceToPropertyContainer(objID);

	// Set metadata
	VRProperties()->SetStringProperty(ulPropertyContainer, Prop_ModelNumber_String, model.c_str());
	VRProperties()->SetStringProperty(ulPropertyContainer, Prop_SerialNumber_String, model.c_str());
	VRProperties()->SetStringProperty(ulPropertyContainer, Prop_TrackingFirmwareVersion_String, version.c_str());
	VRProperties()->SetStringProperty(ulPropertyContainer, Prop_HardwareRevision_String, version.c_str());

	// Tracked device is opting out of left/right hand selection -- According to API documentation
	VRProperties()->SetInt32Property(ulPropertyContainer, Prop_ControllerRoleHint_Int32, ETrackedControllerRole::TrackedControllerRole_OptOut);
	VRProperties()->SetInt32Property(ulPropertyContainer, Prop_DeviceClass_Int32, TrackedDeviceClass_GenericTracker);

	// This describes the path to get to the inputprofile which is ABSOLUTELY NECESSARY.
	// Testing from earlier showed that even if it shouldn't be running the driver is still detected. Maybe when it runs it, reinstall the profile each time?
	VRProperties()->SetStringProperty(ulPropertyContainer, Prop_InputProfilePath_String, "{NLOS}/input/tracker_profile.json");

	// Placeholder frame; credit to https://github.com/m9cd0n9ld/IMU-VR-Full-Body-Tracker
	VRProperties()->SetStringProperty(ulPropertyContainer, Prop_RenderModelName_String, "{NLOS}/rendermodels/frame");

	VRProperties()->SetStringProperty(ulPropertyContainer, Prop_ManufacturerName_String, "NLOS");

	VRProperties()->SetBoolProperty(ulPropertyContainer, Prop_HasDisplayComponent_Bool, false);
	VRProperties()->SetBoolProperty(ulPropertyContainer, Prop_HasCameraComponent_Bool, false);
	VRProperties()->SetBoolProperty(ulPropertyContainer, Prop_HasDriverDirectModeComponent_Bool, false);
	VRProperties()->SetBoolProperty(ulPropertyContainer, Prop_HasVirtualDisplayComponent_Bool, false);

	// ADD LABELS (model, model number, etc)

	// Ignored as hand assignment
	VRProperties()->SetInt32Property(ulPropertyContainer, Prop_ControllerHandSelectionPriority_Int32, -1);

	return VRInitError_None;
}

void TrackerDriver::Deactivate() {
	VRDriverLog()->Log("Shutting down tracker");
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
	switch (TrackerIndex) {
	case (WAIST):
		return waist_pose;
		break;
	case (LKNEE):
		return lknee_pose;
	case (RKNEE):
		return rknee_pose;
	case (LFOOT):
		return lfoot_pose;
		break;
	case (RFOOT):
		return rfoot_pose;
		break;
	default:
		break;
	}
}

void TrackerDriver::RunFrame() {
	if (objID != k_unTrackedDeviceIndexInvalid) {
		VRServerDriverHost()->TrackedDevicePoseUpdated(objID, GetPose(), sizeof(DriverPose_t));
	}
}

void TrackerDriver::setIndex(int limbIndex) {
	this->TrackerIndex = limbIndex;
}

void TrackerDriver::SetModel(std::string model) {
	this->model = model;
}

void TrackerDriver::SetVersion(std::string version) {
	this->version = version;
}
