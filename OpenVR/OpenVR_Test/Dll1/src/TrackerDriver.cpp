#include <TrackerDriver.h>


DriverPose_t last_pose = { 0 };

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

	// Get head mounted display position
	/*
	TrackedDevicePose_t trackedDevicePose;
	IVRSystem* test;
	HmdMatrix34_t poseMatrix;
	HmdVector3_t position;
	HmdQuaternion_t quaternion;

	if (trackedDeviceClass == ETrackedDeviceClass::TrackedDeviceClass_HMD) {
		VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0, &trackedDevicePose, 1);
		HmdMatrix34_t matrix = trackedDevicePose.mDeviceToAbsoluteTracking;
		
		hmd_pose.vecPosition[0] = matrix.m[0][3];
		hmd_pose.vecPosition[1] = matrix.m[1][3];
		hmd_pose.vecPosition[2] = matrix.m[2][3];
		
	}
	*/
	
	char log_str[100];
	TrackedDevicePose_t device_pose[10];
	VRServerDriverHost()->GetRawTrackedDevicePoses(0, device_pose, 10);
	HmdMatrix34_t space_matrix = device_pose[k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;
	double x, y, z;
	x = space_matrix.m[0][3];
	y = space_matrix.m[1][3];
	z = space_matrix.m[2][3];

	hmd_pose.vecPosition[0] = x;
	hmd_pose.vecPosition[1] = y;
	hmd_pose.vecPosition[2] = z;
	

	switch (TrackerIndex) {
	case (WAIST):
		return waist_pose;
		break;
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
